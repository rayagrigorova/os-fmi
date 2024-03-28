#!/bin/bash

# The following lines will make the script safer

# Make the whole script die after the first failure 
set -e 

if [[ ${#} -ne 1 ]]; then
	echo "usage: ${0} <file with planet data>"
	exit 1
fi

if [[! -r "${1}"]]; then
	echo "cannot read from ${1}"
	exit 1
fi

planet_type="$(cat "${1}" | tail -n +2 | sort -t ';' -k 3,3 -g | tail -n 1 | cut -d ';' -f 2)"

cat "${1}" \
	| tail -n +2 \
	| grep -E "^[^;]*;${planet_type};" \
	| sort -t ';' -k 3,3 -g \
	| head -n 1 \
	| cut -d ';' -f 1,4 \
	| tr ';' '\t'
