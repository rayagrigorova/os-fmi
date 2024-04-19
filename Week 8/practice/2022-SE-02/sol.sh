#!/bin/bash

# If I have a choice between more backups I should delete the older ones. 'daily backups are more important than monthly' => if I have the choice to delete 2 monthly files and 2 daily, I will delete the 2 daily files


if [[ "${#}" -ne 2 ]]; then
	echo "Wrong number of args"
	exit 1
fi

if [[ ! -d "${1}" ]]; then
	echo "The first arg should be a directory"
	exit 2
fi

if [[ ! "${2}" =~ ^[1-9][0-9]{,1} ]]; then
	echo "The second arg should be a number between 1 and 99"
	exit 3
fi

# 1. Remove nonexistent symlinks
while read SYMLINK; do
	if [[ ! -e "$(echo ${SYMLINK} | xargs realpath )" ]]; then
		rm "${SYMLINK}"
	fi
done < <(find "${1}" -type l)

# 2. For each hostname-area pair do the following: 
# find the latest yearly, the 2 latest monthly,
# the 3 latest weekly and 4 latest daily backups. 
# Stage the other backups for removal. 
# The  staged for removal backups are saved to a 
# temp file - the information that they contain is 
# a name + file size. Sort the file by size and then 
# by archive type (weekly,daily etc.)
# While $ du --output... doesnt return the wanted result, delete one of the files that were saved to the temp file
while read HOSTANEME_AREA_PAIR; do
# 	echo "${HOSTANEME_AREA_PAIR}"
	TEMP_FILE="$(mktemp -p './tmp' )"

	# Open each one of the directories 
	for i in $(seq 0 3); do
		NEEDED_FILES="$(( ${i}+1 ))"
		echo "$(find "${1}/${i}" -type f -printf "%p %s ${i}\n" | grep -E "${HOSTANEME_AREA_PAIR}" | sort -nr -t '-' -k 3,3 \
			| tail -n +"$(( ${NEEDED_FILES} + 1 ))" )" >> "${TEMP_FILE}"
	done 
	
	while read LINE; do
		# Deleted enough files
		if [[ "$( df --output=pcent "${1}" | tail -n +2 | grep -o -E '[0-9]+' )" -le "${2}" ]]; then
			exit 0 
		fi
		
		if [[ -z "${LINE}" ]]; then
			continue
		fi
		rm -f "$(echo ${LINE} | cut -d ' ' -f 1 )"
	done < <(cat "${TEMP_FILE}" | sort -t ' ' -n -r -k 3 -k 2 )

	rm -f "${TEMP_FILE}"
done <  <(find "${1}" -type f -exec basename {} \; | sed -E 's/^(.*-.*)-[0-9]+\.tar\.xz$/\1/' | sort | uniq)
