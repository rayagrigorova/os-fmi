#!/bin/bash


if [[ "${#}" -ne 3 ]]; then
	echo 'Wrong number of arguments!'
	exit 1
fi


if [[ ! -f "${1}" ]]; then
	echo 'The first argument should be a file'
	exit 2
fi


if [[ -z "${2}" || -z "${3}" ]]; then
	echo 'The key arguments should be non-empty'
	exit 3
fi

if [[ ! -r "${1}" ]]; then
	echo 'The file is not readable'
	exit 4
fi

# Change the terms for key2 if key2 exists in the file 
# If key 2 doesn't exist in the file, dont change anything

# Case when key2 doesn't exist
if ! grep -E -q "^${3}=.*$" "${1}"; then
	exit 0
fi

# 1. Find the lines, get the terms, change the wihitespaces with \n and sort the outputs. Use process subsitution for the outputs of the pipelines
OLD_TERMS="$(grep -E "^${3}=.*$" "${1}" | sed -E "s/^${3}=(.*)$/\1/" )"
# echo "OLD TERMS: ${OLD_TERMS}"

# 2. Using comm, compare the two files and get the keys that are common for both files using comm -1 -2 
while read COMMON_TERM; do
# 	echo "TERM: ${COMMON_TERM}"
	OLD_TERMS="$(echo "${OLD_TERMS}" | sed -E "s/"${COMMON_TERM}"//" )"
done < <(comm -1 -2 <(grep -E "^${3}=.*$" "${1}" | sed -E "s/^${3}=(.*)$/\1/" | \
	tr '[[:blank:]]' '\n' | sort) \
	<(grep -E "^${2}=.*$" "${1}" | sed -E "s/^${2}=(.*)$/\1/" | \
	tr '[[:blank:]]' '\n' | sort))

# Reformat the terms (squeeze whitespaces and remove trailing whitespaces
OLD_TERMS="$(echo "${OLD_TERMS}" | sed -E 's/^[[:blank:]]+//' | tr -s ' ' )"
# The last step is to change the old terms for key2 in the file to the new terms 
sed -E -i "s/^(${3}=).*$/\1${OLD_TERMS}/" "${1}"
# echo "NEW TERMS:${OLD_TERMS}"
