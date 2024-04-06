#!/bin/bash

if [[ -z "${1}" || ! -f "${1}" ]]; then
	echo "Invalid arg (must be a file)"
	exit 1 
fi

INVALID_LINES="$(cat "${1}" | tail -n +2 | grep  -v -E --color '^[a-zA-Z0-9_[:blank:]-]+;[TGI];([0-9]+[,\.]?[0-9]*;){4}[0-9]+[,\.]?[0-9]*$')"

if [[ ! -z "${INVALID_LINES}" ]]; then
	echo "Invalid file format"
	exit 2
fi

MAX_DIST_TYPE="$(cat "${1}" | tail -n +2 | sort -r -n -t ';' -k 3 | head -n 1 | cut -d ';' -f 2 )"
cat "${1}" | tail -n +2 | grep -E --color "^[a-zA-Z0-9_[:blank:]-]+;${MAX_DIST_TYPE};([0-9]+[,\.]?[0-9]*;){4}[0-9]+[,\.]?[0-9]*\$" "${1}" | sort -n -t ';' -k 3 |  cut -d ';' -f 1,4 | head -n 1 | tr ';' '\t'

