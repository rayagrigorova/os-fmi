#!/bin/bash

mkdir -p a b c
if [[ "${#}" -ne 2 ]]; then
	echo "need 2 arguments"
	exit 1
fi


[[ "${#}" -ne 2 ]] || {echo "need 2 args"; exit 1;}


[[! "${1}" =~ ^[0-9]+$ ]] || {echo "need 2 args"; exit 1;}
[[! "${2}" =~ ^[0-9]+$ ]] || {echo "need 2 args"; exit 1;}
[[ "${1}" -le "${2}" ]] || {echo "wrong order"; exit 1;}
[[ -z "$(find . -midepth 1 -type f )"  ]]

while read file_name; do
	lines="$(cat ${file_name}" | wc -l)"

	if [[ "${lines}" -lt "${1}"]]; then
		mv "${file_name}" a
	elif [[ "${lines}" -lt "${2}"]]; then
		mv "${file_name}" b
	else [[ "${lines}" -lt "${3}"]]; then
		mv "${file_name}" c
	fi
done  < <(find . -maxdepth 1 -type f)
