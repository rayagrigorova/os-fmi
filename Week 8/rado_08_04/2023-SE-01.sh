#!/bin/bash

if [[ ! "${#}" == 2 ]]; then
	echo 'Wrong argument number'
	exit 1
fi

if [[ ! -f "${1}" ]]; then
	echo 'The first arg should be a file'
	exit 2
fi

if [[ ! -d "${2}" ]]; then
	echo 'The second arg should be a directory'
	exit 3
fi

# For each bad word 
while read BAD_WORD; do
	# For each file
	while read FILENAME; do
		REPLACEMENT="$(echo ${BAD_WORD} | tr '[a-zA-Z0-9_]' '*' )"		
		sed -i -E "s/${BAD_WORD}/${REPLACEMENT}/g" "${FILENAME}"
	done < <(find ${2} -type f )
done < "${1}"
