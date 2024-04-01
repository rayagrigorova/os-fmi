#!/bin/bash

while read line; do
	# echo "${line}"
	echo $(grep "${line}" /etc/passwd | cut -d ':' -f 5 | sed -E 's/^(([а-яА-Яa-zA-Z]+\s*){2,}),*.*$/\1/')
done < <(ps -e -o euser=,comm= | grep -E '^.*\s*vim\s*$' | sort | uniq -c | awk '$1 >= 2{ print $2}') 
