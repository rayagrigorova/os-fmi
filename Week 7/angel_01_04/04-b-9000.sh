#!/bin/bash

# For each process, find the number of its children and the number 
# of children of the parent process. Compare the two numbers. 
# If the first number is larger than the second, then print

while read LINE; do
	PARENT_ID="$(echo "${line}" | awk '{ print $2 }')"

	CHILDREN_COUNT="$(ps -e -o pid=,ppid= | awk -v AWK_LINE="${LINE}" \
		'$2 == AWK_LINE {print $0}' | wc -l )"
	PARENT_CHILDREN_COUNT="$(ps -e -o pid=,ppid= | awk -v AWK_LINE="${PARENT_ID}" \
		'$2 == AWK_LINE {print $0}' | wc -l )"

	if [[ "${PARENT_CHILDREN_COUNT}" -lt "${CHILDREN_COUNT}" ]]; then
		echo "${LINE}" | awk '{ print $1 }'
	fi
done < <(ps -e -o pid=)
