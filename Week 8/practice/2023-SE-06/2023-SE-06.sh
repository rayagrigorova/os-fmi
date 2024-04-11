#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
	echo 'Wrong argument count'
	exit 1
fi

if [[ ! -d "${1}" ]]; then
	echo 'The first arg should be a directory'
	exit 2
fi

mkdir -p "${2}" # Create the library

# Find the files that are photos and print their paths and mtimes 
# Sort them by their modification time 

# For each of the files found, start a new interval if necessary.
# I will keep a variable containing the name of the currrent interval folder 
# (for example 2023-04-05_2023-04-23). 

TEMP_FILE="$(mktemp)" # I will use this temp file to keep the names of the files that I have to copy inside the currently processed interval. After an interval is processed, this file will be read and emptied 

PREVIOUS_INTERVAL_BEGIN=''
PREVIOUS_INTERVAL_END=''

# It's important that the folders made are in the directory created above for argument 2 of the script
while read PICTURE_FILE; do
	MTIME="$(echo "${PICTURE_FILE}" | cut -d ' ' -f 1 )"
	PATH_TO_FILE="$(echo "${PICTURE_FILE}" | cut -d ' ' -f 2 )"

	if [[ -z "${PREVIOUS_INTERVAL_BEGIN}" ]]; then
		echo "${PATH_TO_FILE}" >> "${TEMP_FILE}" # Save the path in the file
		PREVIOUS_INTERVAL_BEGIN="${MTIME}"
		PREVIOUS_INTERVAL_END="${MTIME}"
	elif [[ "${PREVIOUS_INTERVAL_END}" != "${MTIME}" && "$(date -d "${PREVIOUS_INTERVAL_END} + 1 day" +'%Y-%m-%d')" != "$(date -d "${MTIME}" +'%Y-%m-%d')" ]]; then # Can't expand the interval any further, create a new one 
		mkdir "${2}/"${PREVIOUS_INTERVAL_BEGIN}"_"${PREVIOUS_INTERVAL_END}""

		while read FILENAME; do
			NEW_NAME_FOR_FILE="$(date -d "@$(stat "${FILENAME}" --format='%Y')" +'%Y-%m-%d_%T')"'.jpg'
			cp "${FILENAME}" "${2}/"${PREVIOUS_INTERVAL_BEGIN}"_"${PREVIOUS_INTERVAL_END}"/"${NEW_NAME_FOR_FILE}"" 
		done < "${TEMP_FILE}"

		> "${TEMP_FILE}" # Delete all contents of the file
		PREVIOUS_INTERVAL_BEGIN="${MTIME}"
		PREVIOUS_INTERVAL_END="${MTIME}"
		echo "${PATH_TO_FILE}" >> "${TEMP_FILE}" # Save the path in the file
	else # There is a previous interval and it can be expanded by the new photo 
		# Update the name of the existing directory with the new name ...
		PREVIOUS_INTERVAL_END="${MTIME}"
		echo "${PATH_TO_FILE}" >> "${TEMP_FILE}" # Save the path in the file
	fi
done < <(find "${1}" -type f -name '*.jpg' -printf '%TF %p\n' | sort )

if [[ "$(cat "${TEMP_FILE}" | wc -l)" -gt 0 ]]; then
	mkdir "${2}/"${PREVIOUS_INTERVAL_BEGIN}"_"${PREVIOUS_INTERVAL_END}""

	while read FILENAME; do
		NEW_NAME_FOR_FILE="$(date -d "@$(stat "${FILENAME}" --format='%Y')" +'%Y-%m-%d_%T')"'.jpg'
		cp "${FILENAME}" "${2}/"${PREVIOUS_INTERVAL_BEGIN}"_"${PREVIOUS_INTERVAL_END}"/"${NEW_NAME_FOR_FILE}"" 
	done < "${TEMP_FILE}"

	> "${TEMP_FILE}" # Delete all contents of the file
	PREVIOUS_INTERVAL_BEGIN="${MTIME}"
	PREVIOUS_INTERVAL_END="${MTIME}"
	echo "${PATH_TO_FILE}" >> "${TEMP_FILE}" # Save the path in the file
fi

rm -f "${TEMP_FILE}"
