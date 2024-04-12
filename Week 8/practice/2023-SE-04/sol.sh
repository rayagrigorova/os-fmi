#!/bin/bash


if [[ "${#}" -ne 1 ]]; then
	echo "The number of args should be 1"
	exit 1
fi


if [[ ! -d "${1}" ]]; then
	echo "The arg should be a directory"
	exit 2
fi

if [[ ! -x "${1}" ]]; then
	echo "You don't have execute rights for this dir"
	exit 3
fi

# To make all files with the same contents point to the same copy, I will delete n-1 from the n identical files (saving the names of the deleted files and adding their bytes to a sum). Then I will create n-1 hard links to the n-th file that was not deleted 
# I will also have a counter for the number of groups of files that were deduplicated 

# Keep the following variables: original file(the first file with a given hash), PREV_HASH(the previously found hash), CURR_HASH (the hash that is being currently processed), a temporary file with names of files with PREV_HASH `that have to be deleted once all of them are found 
TO_BE_DELETED="$(mktemp)"
PREV_HASH=''
ORIGNAL_FILE=''

GROUP_CTR=0
BYTES_TOTAL=0
while read LINE; do
	CURR_HASH="$(echo ${LINE} | cut -d ' ' -f 1 )"	

	# Either the hash changed or this was the first loop iteration
	# Do the following - execute rm for all the files in TO_BE_DELETED and create hard links with their names (get their names using basename)
	# Finally, empty the temp file TO_BE_DELETED
	if [[ "${CURR_HASH}" != "${PREV_HASH}" ]]; then

		# If there are no files to be deleted 	
		if [[ "$(cat ${TO_BE_DELETED} | wc -l)" -eq 0 ]]; then
			PREV_HASH="${CURR_HASH}"
			ORIGNAL_FILE="$(echo ${LINE} | cut -d ' ' -f 2 )"	
			continue
		fi

		GROUP_CTR="$(( ${GROUP_CTR} + 1))"	
		while read TO_BE_DELETED_FILE; do
# 			echo "This file will be deleted ${TO_BE_DELETED_FILE} "
			BYTES_TOTAL="$(( BYTES_TOTAL + $(stat --format='%s' ${TO_BE_DELETED_FILE}) ))"
			rm -f "${TO_BE_DELETED_FILE}"
			ln "${ORIGNAL_FILE}" "${1}/""$(basename ${TO_BE_DELETED_FILE})"
		done < "${TO_BE_DELETED}"

		> "${TO_BE_DELETED}" # Empty the file 
# 		Reset the variables
		PREV_HASH="${CURR_HASH}"
		ORIGNAL_FILE="$(echo ${LINE} | cut -d ' ' -f 2 )"	
	else 
		echo ${LINE} | cut -d ' ' -f 2 >>"${TO_BE_DELETED}"
	fi
done < <(find "${1}" -type f -exec md5sum '{}' ';' | sort)


if [[ "$(cat ${TO_BE_DELETED} | wc -l)" -ne 0 ]]; then
	GROUP_CTR="$(( ${GROUP_CTR} + 1))"	
	while read TO_BE_DELETED_FILE; do
# 		echo "This file will be deleted ${TO_BE_DELETED_FILE} "
		BYTES_TOTAL="$(( BYTES_TOTAL + $(stat --format='%s' ${TO_BE_DELETED_FILE}) ))"
		rm -f "${TO_BE_DELETED_FILE}"
		ln "${ORIGNAL_FILE}" "${1}/""$(basename ${TO_BE_DELETED_FILE})"
	done < "${TO_BE_DELETED}"
fi

rm -f "${TO_BE_DELETED}"

echo "Groups deduped: ${GROUP_CTR}"
echo "Total bytes: ${BYTES_TOTAL}"
