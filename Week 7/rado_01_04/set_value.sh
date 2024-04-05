#!/bin/bash

# First check if the arguments are 3, if the file exists and validate 
# the format of the file (use a regular expression to check the file format)
if [[ "${#}" -ne 3 ]]; then 
	echo "The number of arguments you have entered is invalid"
	exit 1
fi

if [[ ! -f "${1}" ]]; then
	echo "The file doesn't exist"
	exit 2
fi

if [[ ! -r "${1}" ]]; then
	echo "You don't have read rights for the file"
	exit 3
fi

word_regex="[[:alnum:]]+"

if ! echo "${2}" | grep -q -E "${word_regex}"; then
	echo "Second argument must be a valid word"
	echo 4
fi

line_regex="^\s*${word_regex}\s*=\s*${word_regex}\s*$"

# For each row we have to check if it matches a regex (the easiest solution is to use grep -v!!)
if cat "${1}" | cut -d '#' -f 1 | grep -E -v '^\s*$' | grep -E -q -v "${line_regex}"; then
	echo "Wrong format of file"
	exit 5
fi

# while read line; do 
# 	echo "${line}"
# done < "${1}"

val=$(grep -q -F "${2}" "${1}")
if [[ -z "${val}" ]]; then
	echo "${2} = ${3} # added by $(whoami) at $(date)" >> "${1}"
else
	sed -i -E "s/^${line_regex}$/#&# edited .../"
fi
