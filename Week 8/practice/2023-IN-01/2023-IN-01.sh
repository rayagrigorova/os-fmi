#!/bin/bash
USE_SLOTS=0

if [[ "${#}" -ge 2 ]]; then
	echo "The number of arguments should be 1 or 2"
	exit 1
fi

if [[ "${#}" == 1 ]] && [[ "${1}" != 'autoconf' ]] && [[ "${1}" != 'config' ]]; then
	echo "The argument should be left empty or be autoconf/config"
	exit 2
fi


if [[ -v "${CTRLSLOTS}" ]]; then
	USE_SLOTS="${CTRLSLOTS}"
fi


