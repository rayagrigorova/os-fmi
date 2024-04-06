#!/bin/bash

if [[ -z "${1}" ]] || [[ ! -f "${1}" ]]; then
	echo "Invalid argument for file" >&2
	exit 1
fi

START_TIME=1551168000
END_TIME=1551176100

# 1. find all directories from /etc/passwd
# 2. for each one of them use find's printf's flags %Ck and %p (xargs) 

while read LINE; do
	CHANGE_TIME="$(echo "${LINE}" | cut -d ':' -f 6 | xargs find / -type d -name {} 2>/dev/null -printf '%C@\n' )"
	
	if [[ "${CHANGE_TIME}" -ge "${START_TIME}" && "${CHANGE_TIME}" -le "${END_TIME}" ]]; then
		echo "${LINE}" | cut -d ':' -f 1,4 | sed -E 's/^s(.*)/\1/' | sed -E 's/^([0-9]+\s+[a-zA-Z[:blank:]-]+).*$/\1/'
	fi
done < "${1}"
