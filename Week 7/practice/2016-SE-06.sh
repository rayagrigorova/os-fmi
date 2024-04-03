#!/bin/bash
 
# If the number of arguments is not 1 or the argument is not a file, exit
if [[ "${#}" -ne 1 || ! -f "${1}" ]]; then
	echo "The arguments you have entered are invalid"
	exit 1
fi

YEAR_REGEX='[0-9]+\s*г\.\s*'
BOOK_NAME_REGEX='[„"“][[:blank:]a-zA-Z0-9а-яА-Я-]+[“„"]'
AUTHOR_NAME_REGEX='[[:blank:]a-zA-Zа-яА-Я.-]+'
BRACKETS_REGEX='\s*\([[a-zA-Zа-яА-Я0-9[:blank:][:punct:]]*\)\s*'

VALID_LINE_REGEX="^${YEAR_REGEX}-\s*${BOOK_NAME_REGEX}\s*(-\s*${AUTHOR_NAME_REGEX})?(${BRACKETS_REGEX})*$"

while read LINE; do
	echo "Line to be processed: ""${LINE}"

	if echo "${LINE}" | grep -E -v -q "${VALID_LINE_REGEX}"; then 
		echo "The file contains invalid lines"
		exit 2 
	fi
done < "${1}"

cat "${1}" | sed -E "s/^${YEAR_REGEX}\s*-\s*(.*)$/\1/" | awk -v ORS='' '{ print NR".", $0,"\n"}' | sort -t ' ' -k 2 
