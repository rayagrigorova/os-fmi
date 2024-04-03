#!/bin/bash

if [[ "${#}" -ne 3 ]]; then
	echo "The number of arguments is wrong"
	exit 1
fi

if [[ ! -f "${1}" ]]; then
	echo "The first argument should be a file name"
	exit 2
fi

if [[ -z "${2}" || -z "${3}" ]]; then
	echo "The keys should be non-empty strings"
	exit 3
fi

STR1_LINE=''
STR2_LINE=''

# If the file doesn't contain a row with str2, then don't change nothing and exit
# (since the row that the script has to change doesn't exist)
while read LINE; do
	# Check if the format of the file is correct. If one line fails the test, then 
	# the format of the whole file is invalid
	if [[ ! "${LINE}" =~ ^[[:alnum:]_-]+[[:blank:]]*=([[:alnum:]_ -]+)*$ ]]; then
		echo "Invalid line" "${LINE}"
		echo 'The file is in invalid format'
		exit 4
	fi

	if [[ "${LINE}" =~ ^"${2}"\s*=\s*([a-zA-Z0-9_ -]+)* ]]; then
		STR1_LINE="${LINE}"
		echo "Line for str1:""${LINE}"
	fi

	if [[ "${LINE}" =~ ^"${3}"\s*=\s*([a-zA-Z0-9_ -]+)* ]]; then
		STR2_LINE="${LINE}"
		echo "Line for str2:""${LINE}"
	fi
done < "${1}"

if [[ -z "${STR2_LINE}" ]]; then
	exit 0;
fi

TERMS_1="$(echo ${STR1_LINE} | sed -E 's/^.*[[:blank:]]*=([[:alnum:]_ -]+)$/\1/')"
# echo "Terms 1:""${TERMS_1}"

TERMS_2="$(echo ${STR2_LINE} | sed -E 's/^.*=(.*)$/\1/')"
# echo "Terms 2:""${TERMS_2}"

for TERM in ${TERMS_1}; do
	echo "${TERM}"
	TERMS_2="$(echo ${TERMS_2} | sed -E "s/${TERM}//")"
done

sed -E -i "s/${STR2_LINE}/${3}=${TERMS_2}/" "${1}"
