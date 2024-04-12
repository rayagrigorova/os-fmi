#!/bin/bash

# Use a hash sum to determine if the contents of the files are the same 

TEMP="$(mktemp -d)"
# tmp        result of cat 
# hash1   		1232
# 
# hash2 			4561
#  				7891

while read INODE NUM FILENAME; do
	HASH="$(md5sum ${FILENAME} |  cut -d ' ' -f 1)"
	echo "${INODE}" "${NUM}" >> "${TEMP}/${HASH}"
done < <(find "${1}" -type f -printf '%i %n %p\n')

while read GROUP_FILE; do
	FILES="$(cat ${GROUP_FILE} | sort | uniq )"
	COUNT="$(echo ${FILES} | cut -d ' ' -f 2 )"
# 	If the file contains only 1s then we only have double files and we can get all the files except one and delete the files with the other inodes 

done < <(find "${TEMP}" -type f )
rm -rf "${TEMP}"
