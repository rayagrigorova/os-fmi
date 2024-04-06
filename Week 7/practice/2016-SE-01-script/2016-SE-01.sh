#!/bin/bash
if [[ -z "${1}" ]]; then
	echo "Missing parameter" >&2
	exit 1
fi

if [[ ! -d "${1}" ]]; then
	echo "Not a directory" >&2
	exit 2
fi

CTR=0
while read LINE; do
# 	echo "LINE: ${LINE}"
	if [[ ! -e "${LINE}" ]]; then
# 		echo "Not working link path: ${LINE} "
		CTR="$(( ${CTR} + 1 ))"
	fi
done < <(find "${1}" -mindepth 1 -maxdepth 2 -type l -exec readlink {} ';')

echo "Number of broken links is ${CTR}"
