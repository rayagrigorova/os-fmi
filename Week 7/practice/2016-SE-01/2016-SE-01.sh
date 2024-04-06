#!/bin/bash

if [[ -z "${1}" || ! -f "${1}" ]]; then
	echo "The first arg should be a file name" 
	exit 1
fi

# LINE_REGEX=''
# CTR=0
# while read LINE; do
# 	if [[ " ${LINE}" =~ ^[^a-w]*[02468]+[^a-w]*$ ]]; then
# 		CTR="$(( ${CTR} + 1 ))"
# 	fi
# 		
# done < "${1}"
# echo "${CTR}"


cat "${1}" | awk '$0 ~ /^[^a-w]*[02468]+[^a-w]*$/ {ctr += 1} END {print ctr}'
