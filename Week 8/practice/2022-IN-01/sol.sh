#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
	echo "The number of args should be 2"
	exit 1
fi

if [[ ! -d "${1}" || ! -d "${2}" ]]; then
	echo "The args should be direcotries"
	exit 2
fi

if [[ "$(find "${2}" -mindepth 1 | wc -l)" -gt 0 ]] ; then
	echo "The second directory is non empty"
	exit 3
fi
# 1. Create the directory structure 

SRC_DIR_NAME="$(echo "${1}" | xargs realpath | xargs basename)"
echo "SRC: ${SRC_DIR_NAME}"

find "${1}" -mindepth 1 -type d | xargs realpath | sed -E "s:^.*${SRC_DIR_NAME}/(.*)$:\1:" | xargs -I {} mkdir -p "${2}/"{}

# 2. Find all the files in the src directory 

# 3. For each of the saved filed check if it's a swap file. If not, copy it # If yes, check if the temp file contains a file with the same name as the swap file. If yes, skip it. The copying will be done the following way: the source is the file path from the temp file. The destination will be obtained using the SED regex I wrote above. 
TO_BE_COPIED="$(mktemp)"
TEMP_FILE="$(mktemp)"
find "${1}" -type f -exec realpath {} ';' > "${TEMP_FILE}"
while read FILE_PATH; do
	# A swap file was found - check for corresponding original file
	# By original file I mean a file that caused the creation of the swap file
	if echo "${FILE_PATH}" | xargs basename | grep -q -E '^\..*\.swp$'; then
		ORIGINAL_FILE="$( echo ${FILE_PATH} | sed -E 's:^(.*)\.(.*)\.swp$:\1\2:' )"
		echo "ORIGINAL: ${ORIGINAL_FILE}"
		# If there exist a file that the swp file relates to
		if grep -E -q "${ORIGINAL_FILE}" "${TEMP_FILE}"; then
			continue
		fi
	fi
	echo "${FILE_PATH}" >> "${TO_BE_COPIED}"
done < "${TEMP_FILE}"

echo 'TO CP'
cat "${TO_BE_COPIED}" 
# Execute cp for each line with src=the line and dest=the same line, but modified with sed (replace everything up to SRC_DIR_NAME with the path to DEST_DIR)
while read LINE; do
# 	echo ${LINE} | sed -E "s:^.*${SRC_DIR_NAME}(.*)$:${2}\1:" 
	cp "${LINE}" "$(echo ${LINE} | sed -E "s:^.*${SRC_DIR_NAME}(.*)$:${2}\1:" )"
done < "${TO_BE_COPIED}"

rm -r "${TO_BE_COPIED}"
