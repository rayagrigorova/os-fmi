#!/bin/bash

CURRENT_USER="$(whoami)"
if [[ "${CURRENT_USER}" != 'root' ]]; then
	echo "This script should be executed by root"
	exit 1
fi

USERS_NO_HOMEDIR="$(cat /etc/passwd | grep -E '^([a-zA-Z0-9_/-]*:){5}:[a-zA-Z0-9_/-]*$'\
	| cut -d ':' -f 1)"
USERS_CANT_WRITE="$(cat /etc/passwd | cut -d ':' -f 6 | xargs stat -c %A | grep -E '^.-.*' )"

echo "${USERS_NO_HOMEDIR}"
echo "${USERS_CANT_WRITE}"
