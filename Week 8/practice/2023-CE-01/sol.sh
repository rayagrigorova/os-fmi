#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
	echo "Wrong arg count"
	exit 1
fi


if [[ ! -f "${1}" ]]; then
	echo "The first arg should be a file"
	exit 2
fi

if [[ ! -r "${1}" ]]; then
	echo "The file is not readable"
	exit 3
fi

if [[ -z "${2}" ]]; then
	echo "The star type string should be non empty"
	exit 4
fi

# 1. Find the constellation with the most stars with the type given by the second param  (I will do this using awk and arr[consellation_name], which will be incremented each time a star with the target type is found)
NEEDED_CONSTELLATION="$(cat ${1} | awk -F ',' -v TARGET_TYPE=${2} '$4!="--" && $5!="--" && $5==TARGET_TYPE {arr[$4]+=1} END{for (key in arr) print arr[key],key}' | sort -nr | head -n 1 | cut -d ' ' -f 2)"
# echo "${NEEDED_CONSTELLATION}"

# 2. For the constallation found, find the brightest star using grep and then sort -k 

BRIGTEST_STAR="$(cat ${1} | grep -E \
	"^(.*,){3}${NEEDED_CONSTELLATION},(.*,){2}.*$" | grep -v -E '^.*,--$' | sort -t ',' -k 7 -r -g | head -n 1 | cut -d ',' -f 1 )"
echo "${BRIGTEST_STAR}"

