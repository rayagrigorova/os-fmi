#!/bin/bash

COUNT="$(ps -u ${1} -o comm= | wc -l)"
TEMP_FILE="$(mktemp)"
ps -e -o user=,etimes=a  > "${TEMP_FILE}"
cat "${TEMP_FILE}" | xargs -L 1  | cut -d ' ' -f 1 | sort | uniq -c | awk "$1 > COUNT "
# pass throught etc passwd 
# for each user check the number of processes. If the number of processes for the user is greater than this of foo, then echo the name 

TOTAL_SUM="$(cat ${TEMP_FILE} | cut -d ' ' -f 2 | xargs | tr ' ' '+' | bc )"
TOTAL_COUNT="$(cat ${TEMP_FILE} | wc -l )
AVERAGE="$(echo "${TOTAL_SUM} / ${TOTAL_COUNT}" | bc )"
date -u -d "@${AVERAGE}" +%H:%M:%S

# For the third part of the problem, use ps -e -o time=


rm -f "${TEMP_FILE}"
