#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
	echo 'Wrong arg count' >&2
	exit 1
fi

if [[ ! -d "${1}" ]]; then
	echo 'The first arg should be a directory' >&2
	exit 2
fi


if [[ ! -x "${1}" ]]; then
	echo 'The directory should have execute rights'
	exit 3
fi

find "${1}" -maxdepth 1 -type f -exec basename {} ';' | grep -E "^(.*)-(([0-9]+)?\.([0-9]+)?\.([0-9]+)?)-${2}$" | sed -E "s:^(.*)-(([0-9]+)?\.([0-9]+)?\.([0-9]+)?)-${2}$:\2 &:" | sort -n -r | head -n 1 | awk '{print $2}'
