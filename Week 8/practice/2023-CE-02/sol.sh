#!/bin/bash

if [[ "${#}" -ne 3 ]]; then
	echo "Wrong arg count"
	exit 1
fi

if [[ ! -f "${1}" ]] || [[ ! -f "${2}" ]]; then
	echo "The first 2 args must be files"
	exit 2
fi

if [[ -z "${3}" ]]; then
	echo "The name of the black hole must be non empty"
	exit 3
fi

if [[ ! -r "${1}" ]] || [[ ! -r "${2}" ]]; then
	echo "You dont have read permissions for the two files"
	exit 4
fi

FIRST_DIST="$(grep -E "${3}" "${1}" | \
	sed -E 's/^.*:\s*([0-9]+)\s*megaparsecs\s*$/\1/')"
SECOND_DIST="$(grep -E "${3}" "${2}" |\
	sed -E 's/^.*:\s*([0-9]+)\s*megaparsecs\s*$/\1/')"

if [[ "${FIRST_DIST}" -le "${SECOND_DIST}" ]]; then
	echo "${1}"
	exit 0
fi
echo "${2}"
exit 0
