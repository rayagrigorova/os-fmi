#!/bin/bash

# Config file in ARKCONF
# Check for validity

# Determine what to do (what rsycn command to use)
# For eacj ==h server/given server 
# -what will be synced
# -confirm
# sync
export ARKCONF='./arkconf.conf'
DIRECTION=''
FLAG_D=''
TARGET_SERVER=''

for ARG in "${@}"; do
	if [[ "${ARG}" == 'push' ]] || [[ "${ARG}" == 'pull' ]] ; then
		if [[ -n "${DIRECTION}" ]]; then
			echo 'Direction already set'
			exit 1
		fi
		DIRECTION="${ARG}"
		continue
	fi

	if [[ "${ARG}" == '-d' ]]; then
		FLAG_D="${ARG}"
		continue
	fi
	TARGET_SERVER="${ARG}"
done;

if [[ -z "${DIRECTION}" ]]; then
	echo 'Direction not set'
	exit 2
fi

# echo "DIRECTION: "${DIRECTION}" "
# echo "TARGET_SERVER: "${TARGET_SERVER}" "
# echo "FLAG_D: "${FLAG_D}" "

# To do the synchronization, the option -a should be used 

if [[ -z "${ARKCONF}" ]]; then
	echo 'Config file not set'
	exit 2
fi

# if [[ -r "${ARKCONF}" ]]; then
# 	echo 'Config file cant be read'
# 	exit 3
# fi

CONFIGURED_PATH="$(cat ${ARKCONF} | grep -E '^WHAT=' | cut -d '=' -f 2 | sed -E 's/"//g' )"
CONFIGURED_USER="$(cat ${ARKCONF} | grep -E '^WHO=' | cut -d '=' -f 2 | sed -E 's/"//g' )"
CONFIGURED_SERVERS="$(cat ${ARKCONF} | grep -E '^WHERE=' | cut -d '=' -f 2 | sed -E 's/"//g' )"

SERVERS_TO_SYNC="${CONFIGURED_SERVERS}"
if [[ -n TARGET_SERVER ]]; then
	SERVERS_TO_SYNC="${TARGET_SERVER}"

	if ! echo "${CONFIGURED_SERVERS}" | grep -qwF  "${TARGET_SERVER}"; then
		echo "${TARGET_SERVER} not configured"
	fi
fi

for SERVER in "${SERVERS_TO_SYNC}"; do
	LOCAL_PATH="$(CONFIGURED_PATH)"
	REMOTE_PATH="${CONFIGURED_USER}@${SERVER}:${CONFIGURED_PATH}"
	SRC=""
	DEST=''

	if [[ "${DIRECTION}" == push ]]; then
		SRC="${LOCAL_PATH}"
		DEST="${REMOTE_PATH}"
	else 
		DEST="${LOCAL_PATH}"
		SRC="${REMOTE_PATH}"
	fi

	echo rsync -a "${FLAG_D}" -vn "${SRC}/" "${DEST}/"  
	read -p 'Do  you want to continue?' answer 
	echo rsync -a "${FLAG_D}" "${SRC}/" "${DEST}/"
done
