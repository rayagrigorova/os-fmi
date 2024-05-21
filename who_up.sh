#!/bin/bash

temp_file="$(mktemp)"
who | awk '{print $1}' | sort | uniq > "${temp_file}"

while read line; do
	RESULT="$(grep -E "${line}" /etc/passwd | cut -d ':' -f '5' | grep -E -o '^([а-яА-Яa-zA-Z]+\s*){1,}')"
	
	if [[ "${RESULT}" == 'Георги Балтиев' ]]; then
		echo 'gosho e onlain moje da go bullyvash'
	else 
		echo "${RESULT}"
	fi
done < "${temp_file}"

rm -f "${temp_file}"
