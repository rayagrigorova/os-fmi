#!/bin/bash


if [[ "${#}" -ne 2 ]]; then
	echo 'Wrong arg count' >&2
	exit 1 
fi

if [[ ! -f "${1}" || ! -f "${1}" ]]; then
	echo 'Both arguments must be files' >&2
	exit 2
fi

if [[ ! -r "${1}" ]]; then
	echo 'The first file should be readabale' >&2
	exit 3
fi

if [[ ! -w "${2}" ]]; then
	echo 'The second file should be writeable' >&2
	exit 4
fi

PREV_SET=''
while read LINE; do
	CURR_SET="$(echo "${LINE}" | cut -d ',' -f 2- )"	

	# Echo the first ocurrance of a given set in the file; ignore all the
	# members of the set that come after it as they are sorted by id 
	if [[ "${CURR_SET}" != "${PREV_SET}" ]]; then
		echo "${LINE}" >> "${2}"
		PREV_SET="${CURR_SET}"
	fi
done < <(sort -t ',' -k 2 -k 1,1n "${1}")


