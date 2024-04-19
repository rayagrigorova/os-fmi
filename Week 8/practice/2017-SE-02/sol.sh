#!/bin/bash

# 0. Data validation - check for 3 args, 2 of them 
# directories, permissions, empty string, non-empty second directory

# Also, check for root 

# if [[ "$(whoami)" != 'root' ]]; then
# 	echo "Ths command should be executed by root" >&2
# 	exit 6
# fi

if [[ "${#}" -ne 3 ]]; then
	echo "The number of args should be 3" >&2
	exit 1
fi


if [[ ! -d "${1}" ]] ||  [[ ! -d "${2}" ]]; then
	echo "The first 2 args should be directories" >&2
	exit 2
fi

if [[ ! -r "${1}" ]] ||  [[ ! -x "${2}" ]] || [[ ! -r "${2}" ]] ||  [[ ! -x "${2}" ]]; then
	echo "You dont have permissions for the directories" >&2
	exit 3
fi

if [[ -z "${3}" ]]; then
	echo "The string should be non-empty" >&2
	exit 4
fi

if [[  "$(find "${2}" -mindepth 1 | wc -l)" -gt 0 ]]; then
	echo "The second directory should be empty" >&2
	exit 5
fi

# 1. Find the files in SRC, the name of which contains the string ABC as a substring. 
# 2. For each of them use realpath to get the actual path
# Remove everything up to the name of SRC directory inclusively. Add 
# DEST to the beginning of the result string. This will be used 
# to create the actual directory structure using mkdir -p
# 3. Move the file from the SRC directory to the newly created sub directory of DTS

SRC_NAME="$(realpath ${1} | xargs basename )"
# echo "${SRC_NAME}"
DEST_NAME="$(realpath ${2} | xargs basename )"
# echo "${DEST_NAME}"

while read FILENAME; do
	REAL_PATH_TO_FILE="$( realpath ${FILENAME} )"
	PATH_IN_DEST="$(realpath ${2})""$(echo ${REAL_PATH_TO_FILE} | sed -E "s:^.*${SRC_NAME}(.*)/.*$:\1:" )"
# 	echo "${PATH_IN_DEST}"
	mkdir -p "${PATH_IN_DEST}"
	mv "${REAL_PATH_TO_FILE}" "${PATH_IN_DEST}"
done < <(find "${1}" -type f -name "*${3}*" )

 
