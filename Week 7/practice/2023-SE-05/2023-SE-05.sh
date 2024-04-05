#!/bin/bash

MAX_RSS=65536

# 0. Have an infinate while loop that counts the number of times it has been executed and sleeps 1 second
# 1. For each cmd find the total rss 
# 2. Sort the total rss's and commands by rss size(descending)
# 3. Check the head of the temp file where the rss sums are saved. If it's smaller than MAX_RSS, then quit the loop
# 4. Else, start processing the lines until the rss sum becomes too little 
# 5. Keep a second file with the command names and number of times they have exceeded MAX_RSS
# After the while loop, compare the number of times when a command had >= MAX_RSS and print the commands that satisfy this 

LOOP_REPS=0
LOOKUPS_COUNT_FILE="$(mktemp)"
while true; do
	if [[ "${LOOP_REPS}" -gt 4 ]]; then
		break;
	fi
	LOOP_REPS="$(( "${LOOP_REPS}" + 1 ))"

	PREV_COMM=''
	TOTAL_SUM=0
# 	This file contains the sums of the rss for each command from ps 
	SUMS_FILE="$(mktemp)"

	while read LINE; do 
		CURR_CMD="$(echo ${LINE} | sed -E "s/(^[a-zA-Z0-9[:blank:][:punct:]]+)\s+[0-9]+$/\1/")"
		CURR_RSS="$(echo ${LINE} | sed -E "s/^[a-zA-Z0-9[:blank:][:punct:]]+\s+([0-9]+$)/\1/")"
		
		if [[ -z "${PREV_COMM}" ]]; then
			PREV_COMM="${CURR_CMD}"
		fi
		if [[ "${PREV_COMM}" != "${CURR_CMD}" ]]; then
			echo "${TOTAL_SUM} ${PREV_COMM}" >> "${SUMS_FILE}"
			PREV_COMM="${CURR_CMD}"
			TOTAL_SUM="${CURR_RSS}"
		else 
			TOTAL_SUM="$(( "${TOTAL_SUM}" + "${CURR_RSS}" ))"
		fi
	done < <(ps -e -o comm=,rss= | sort ) 

 	sort -n -r -o "${SUMS_FILE}" "${SUMS_FILE}"
 	
	if [[ "$(head -n 1 "${SUMS_FILE}" | sed -E 's/^([0-9]+).*/\1/')" -lt "${MAX_RSS}" ]]; then
		echo "There aren't any more commands with sum of rss >= ${MAX_RSS} "
		break;
	fi

	while read LINE; do 
		CURR_RSS_SUM="$(echo ${LINE} | sed -E 's/^([0-9]+)\s+[^[:blank:]].*$/\1/' )"
		 
		if [[ "${CURR_RSS_SUM}" -lt "${MAX_RSS}" ]]; then
			break
		fi

		# See if the command is already contained in the file 
		CURR_CMD="$(echo ${LINE} | sed -E 's/^[0-9]+\s+([^[:blank:]].*)$/\1/' )"

		PREV_COUNT="$(cat ${LOOKUPS_COUNT_FILE} | grep -E "${CURR_CMD}" | sed -E 's/^([0-9]+)\s+.*$/\1/')"

		if [[ -z "${PREV_COUNT}" ]]; then
			echo "1 ${CURR_CMD}" >> "${LOOKUPS_COUNT_FILE}"
		else 
			PREV_COUNT="$(( ${PREV_COUNT} + 1))"
			sed -E -i "s/^[0-9]+(\s+${CURR_CMD})$/${PREV_COUNT}\1/" "${LOOKUPS_COUNT_FILE}"
		fi
	done < "${SUMS_FILE}"

	sort -n -r -o "${LOOKUPS_COUNT_FILE}" "${LOOKUPS_COUNT_FILE}"

	rm -f "${SUMS_FILE}"
done

while read LINE; do
	CURR_COUNT="$(echo "${LINE}" | sed -E 's/^([0-9]+)\s+.*$/\1/')"
	CURR_CMD="$(echo ${LINE} | sed -E 's/^[0-9]+\s+([^[:blank:]].*)$/\1/' )"

	if [[ "$(( "${CURR_COUNT}" * 2 ))" -ge "${LOOP_REPS}" ]]; then
		echo "${CURR_CMD}"
	fi
done < "${LOOKUPS_COUNT_FILE}"

rm -f "${LOOKUPS_COUNT_FILE}"
