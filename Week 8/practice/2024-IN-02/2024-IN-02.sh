#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
	echo "Wrong arg count"
	exit 1
fi

if [[ ! -x "${1}" ]] || [[ ! -r "${1}" ]] ; then
	echo "You dont have permissions for the directory"
	exit 2
fi


if [[ ! -f "${2}" ]]; then
	echo "The second argument should be a file"
	exit 3
fi

> "${2}"

CLASS_REGEX='^\s*class\s+([a-zA-Z0-9]+)\s*:?((\s*(public|private|protected)\s*[a-zA-Z0-9]+,?)*)?\s*$'
# Cat all the files together in one 
while read LINE; do
	if echo "${LINE}" | grep -E -q "${CLASS_REGEX}" ; then
		CLASS_NAME="$(echo ${LINE} | sed -E "s/${CLASS_REGEX}/\1/" )"
		if ! grep -E -q "${CLASS_NAME}" "${2}"; then
			echo "${CLASS_NAME}" >> "${2}"
		fi
		PARENTS="$(echo ${LINE} | sed -E "s/${CLASS_REGEX}/\2/" | sed -E 's/(public|private|protected)//g' | tr -d ','| xargs)"

		for PARENT in ${PARENTS}; do
			# If the parent doesnt exist as a node 
			if ! grep -E -q "${PARENT}" "${2}"; then
				echo "${PARENT}" >> "${2}"
			fi
			echo "${PARENT}->${CLASS_NAME}" >> "${2}"
		done
	fi
done < <(find ${1} -type f -name '*.h' | xargs cat)
