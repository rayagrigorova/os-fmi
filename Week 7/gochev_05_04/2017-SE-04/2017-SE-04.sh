#!/bin/bash

# checks: at least one arg and the first arg is a directory 
# find all symlinks 
# for each of them check if its destination exists and if yes, print it
# Else, count it as broken 
# Output could be stdout  or a file 

OUT="/dev/stdout"

if [[ "${#}" -lt 1 || "${#}" -gt 2 ]]; then
	echo "must have between 1 and 2 args" >&2
	exit 1
fi
 
if [[ "${#}" -eq 2 ]]; then
	OUT="${2}"
fi

if [[ ! -d "${1}" ]]; then
	echo "${1} is not a directory"
	exit 2
fi

BROKEN=0
while read FILE; do 
	# Check if the symlink is broken 
	if [[ -e "${FILE}" ]]; then
		echo "$(basename ${FILE})""->""$(readlink ${FILE})" >> "${OUT}"
	else 
		broken="$((broken+1))"
	fi
done < <(find "${1}" -type l )

echo "Broken symlinks:""${BROKEN}" >>"${OUT}"
