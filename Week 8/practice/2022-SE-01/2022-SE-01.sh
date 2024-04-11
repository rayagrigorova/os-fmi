#!/bin/bash

# first arg -> config file
# If a device doesnt exist, issue a warning 
WAKEUP_FILE='example-wakeup'

if [[ ! "${#}" == 1 ]]; then
	echo 'Wrong number of args (must be 1)'
	exit 1
fi

if [[ ! -f "${1}" ]]; then
	echo 'The argument is not a file'
	exit 2
fi

if [[ ! -r "${1}" ]]; then
	echo 'You dont have read rights for the given file'
	exit 3
fi

# Algorithm idea: parse the lines from the config file one by one 
# If the line is invalid stop the whole script (this is done before making any changes to the /proc/acpi/wakeup file) 
while read LINE; do
	if [[ ! "${LINE}" =~ ^\s*#.* ]] && [[ ! "${LINE}" =~ ^[A-Z]{1,4}[[:blank:]]+(disabled|enabled)[[:blank:]]*(#.*)?$ ]]; then
		echo 'A line from the config file is in the wrong format.'
		echo "Problem line: ${LINE}"
		exit 4
	fi
done < "${1}" 

# Then, after the initial validation for correct format parse the config file again. Grep for the device name. If it appears in the /proc file, change its status to the given one 
# If it doesn't appear, issue a warning 
while read LINE; do
	# Ignore lines that are comments
	if [[ "${LINE}" =~ ^\s*#.* ]]; then
		continue
	fi
	DEVICE_NAME="$(echo ${LINE} | grep -o -E '^[A-Z]{1,4}' )"
	SETTING="$(echo ${LINE} | sed -E 's/^[A-Z]{1,4}[[:blank:]]+(disabled|enabled)[[:blank:]]*(#.*)?$/\1/' )"
	if [[ "$(grep -E -c ${DEVICE_NAME} ${WAKEUP_FILE} )" -eq 0 ]]; then
		echo "The device ${DEVICE_NAME} doesn't exist"
		continue # No way to change setting for nonexistent device
	fi
	# Otherwise sed in the file 
	sed -i -E "s/^(\s*${DEVICE_NAME}\s+[^[:blank:]]+\s+\*)(enabled|disabled)(\s+[^[:blank:]]+)\s*$/\1${SETTING}\3/" "${WAKEUP_FILE}"
done < "${1}"
