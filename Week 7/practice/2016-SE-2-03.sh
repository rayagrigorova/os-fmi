#!/bin/bash

mkdir -p a b c 

if [[ "${#}" -ne 2 ]]; then 
	echo "The number of arguments you have passed is wrong"
	exit 1
fi

if [[ !  "${1}" =~ ^[0-9]+$ || ! "${2}" =~ ^[0-9]+$ ]]; then
	echo "The format of the arguments is wrong"
	exit 2
fi
	
# FILES_IN_CURRENT_DIR="$(find . -maxdepth 1 -type f)"
# echo "${FILES_IN_CURRENT_DIR}"

while read LINE; do 
	LINES_COUNT="$(cat "${LINE}" | wc -l)"
	if [[ "${LINES_COUNT}" -lt "${1}" ]]; then
		mv "${LINE}" a 
	elif [[ "${LINES_COUNT}" -lt "${2}" ]]; then
		mv "${LINE}" b 
	else 
		mv "${LINE}" c 
	fi
done < <(find . -maxdepth 1 -type f)
