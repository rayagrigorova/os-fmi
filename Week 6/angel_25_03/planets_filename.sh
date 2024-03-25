#!/bin/bash

planet_type="$(cat "${1}" | tail -n +2 | sort -t ';' -k 3,3 -g | tail -n 1 | cut -d ';' -f 2)"

cat "${1}" \
	| tail -n +2 \
	| grep -E "^[^;]*;${planet_type};" \
	| sort -t ';' -k 3,3 -g \
	| head -n 1 \
	| cut -d ';' -f 1,4 \
	| tr ';' '\t'
