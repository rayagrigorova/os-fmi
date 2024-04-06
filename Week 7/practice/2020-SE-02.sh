#!/bin/bash

# CORRECT_LINE_REGEX='^[0-9]+\|[a-zA-Z0-9_[:blank:]-]+\|(Success|Failure)\|[a-zA-Z0-9_[:blank:]-]+$'
if ! [[  "$(cat "${1}" | head -n 1)" =~ ^date\|launch\ site\|outcome\|payload$ ]]; then
	echo "The header of the file is in the wrong format" >&2
	exit 1
fi

while read LINE; do
	if [[ ! "${LINE}" =~ ^[0-9]+\|[a-zA-Z0-9_[:punct:][:blank:]-]+\|(Success|Failure)\|[a-zA-Z0-9_[:blank:][:punct:]-]+$ ]]; then
		echo "PROBLEM LINE ${LINE}"
		echo "At least one of the lines is in invalid format" >&2
		exit 2
	fi
done < <(tail -n +2 "${1}")

MOST_UNSUCC_SITE="$(tail -n +2 "${1}" | awk -v FS='|' '$3 == "Failure" { unsucc[$2] += 1 } END { for (key in unsucc) print unsucc[key], key }' | sort -n -r | \
	head -n 1 | cut -d ' ' -f 2)"
# echo "${MOST_UNSUCC_SITE}"
grep --color -E "^[0-9]+\|${MOST_UNSUCC_SITE}\|[a-zA-Z]+\|[a-zA-Z0-9[:blank:][:punct:]]+$" "${1}" | sort -n -r | head -n 1 | cut -d '|' -f 3,4 | tr '|' ':'

