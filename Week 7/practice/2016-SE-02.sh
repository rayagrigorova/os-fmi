#!/bin/bash

# Validation checks
if [[ "${#}" -ne 1 || ! "${1}"  =~ ^[0-9]+$ ]]; then
	echo "The arguments you have entered are invalid"
	exit 1
fi

CURRENT_USER="$(whoami)"
if [[ "${CURRENT_USER}" != "root" ]]; then
	echo "The script must be run as root"
	exit 2
fi

TEMPORARY_FILE="$(mktemp)"
ps -e -o uid,pid,rss> "${TEMPORARY_FILE}"
# Print the file 
cat "${TEMPORARY_FILE}" 

# Sort the file so that processes from the same user occurr one after another
cat "${TEMPORARY_FILE}" | tail -n 2 | sort > "${TEMPORARY_FILE}"

CURRENT_SUM=0
CURRENT_USER=""
CURRENT_MOST_RSS=0
CURRENT_MOST_PID=""

function perform_user_switch(){
	# This function will be used whenever the user that is currently being processed changes
	# Check if the sum exceeds the $1 parameter, if yes, kill the most memory heavy one
	# Reset all variables

	CURRENT_USER="$1"
	if [[ "${CURRENT_SUM}" -gt "${1}" ]]; then
		kill "${CURRENT_MOST_PID}"
	fi
	CURRENT_SUM=0
	CURRENT_USER=""
	CURRENT_MOST_RSS=0
	CURRENT_MOST_PID=""
}


while read LINE; do
	ITER_USER="$(echo "${LINE}" | awk '{ print $1 }' )"	
	if [[ "${ITER_USER}" != "${CURRENT_USER}" ]]; then
		perform_user_switch "${ITER_USER}"
	fi
	
	CURRENT_SUM=$(( CURRENT_SUM + "$( echo "${LINE}" | awk '{print $3')" )) 
	THIS_RSS="$( echo "${LINE}" | awk '{print $3')"
	if [[ "${CURRENT_MOST_RSS}" -lt "${THIS_RSS}" ]]; then
		CURRENT_MOST_RSS="${THIS_RSS}"
		CURRENT_MOST_PID="$( echo "${LINE}" | awk '{print $2')"
	fi
done < "${TEMPORARY_FILE}"

rm -f "${TEMPORARY_FILE}"

