#!/bin/bash

NUMBER_OF_PROCESSES=0
TOTAL_SUM=0

while read LINE; do
	NUMBER_OF_PROCESSES=$(( "${NUMBER_OF_PROCESSES}" + 1 ))
	TOTAL_SUM=$(( "${TOTAL_SUM}" + "${LINE}" )) 
	
done < <(ps -g "root" -o rss=) 

if  [[ "${NUMBER_OF_PROCESSES}" -eq 0 ]]; then 
	echo 0
	exit 0
fi

echo "$(( "${TOTAL_SUM}" / "${NUMBER_OF_PROCESSES}" ))" 
