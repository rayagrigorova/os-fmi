#!/bin/bash

CMDS="$(mktemp)"
COUNT=65536
CTR=0
while true;do
	CTR="$(( ${CTR} + 1 ))"
	for COMMAND in COMMS; do
		ps -e -o comm="${COMMAND}",rss= | grep -E "^${COMMAND} " | cut -f e | xargs | tr -d '[[:blank:]]' | bc 
		if [[  "${SUM}" -gt "${COUNT}" ]]; then
			echo ${COMMAND} >> "${CMDS}"
			BREAK=false
		fi
		if [[ BREAK ]]; then
			break;
		fi
	done
done
cat "${CMDS}" | sort | uniq -c | awk "$1 > ${COUNT}"
