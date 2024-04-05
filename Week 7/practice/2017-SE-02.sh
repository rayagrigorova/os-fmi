#!/bin/bash

if [[ "${#}" -ne 3 ]]; then 
	echo "The number of arguments you have entered is wrong"
	exit 1
fi


if [[ "${#}" -ne 3 ]]; then 
	echo "The number of arguments you have entered is wrong"
	exit 1
fi

if ! file "${1}" | grep -E '.*directory.*'; then
	echo "The first argument is not a directory"
	exit 2
fi

if ! file "${2}" | grep -E '.*directory.*'; then
	echo "The second argument is not a directory"
	exit 3
fi

if [[ "$(find "${1}" -type f | wc -l)" -ne 0 ]] ; then
	echo "The source dir should be empty"
	exit 4
fi

# CURR_USER="$(whoami)"
# if [[ "${CURR_USER}" != "root" ]]; then
# 	echo "This script should be executed by root"
# 	exit 5;
# fi


