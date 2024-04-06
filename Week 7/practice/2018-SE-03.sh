#!/bin/bash

if [[ -z "${1}" || ! -f "${1}" ]];then
	echo "Invalid argument (it should be a file)"
	exit 1
fi

SI_GROUP="$(cat "${1}" | sort -n -t : -k 3,3 | head -n 201 | tail -n 1 | cut -d ':' -f 4 )"

grep -E "^([^:]+:){3}"${SI_GROUP}":([^:]+:){2}[^:]+$" "${1}" | sort -n -t ' ' -k 1 | cut -d ':' -f 5-6 | sed -E 's/^([a-zA-Z-]+\s+[a-zA-Z-]+).*(:.*)$/\1\2/'
