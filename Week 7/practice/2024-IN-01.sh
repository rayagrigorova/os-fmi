#!/bin/bash

# if [[ ! -f "${ARKCONF}" ]]; then
# 	echo "The env variable is not set correctly"
# 	exit 1 
# fi

# WHAT contains the dirname to sync(same on all machines)
# WHERE contains server names delimeted by spaces
# WHO is username 
# The file name is in the env var ARKCONF

# I am first going to take the options with which the command was called 
echo "${@}"
PUSH_PULL="$(echo "${@}" | grep -o -E 'push|pull')"

if [[ -z "${PUSH_PULL}" ]]; then
	echo "The command needs a push or pull arg"
	exit 1 
fi

DELETE_OPT="$(echo ${@} | grep -o -E -- '-d')"
SERVER_OPT="$(echo ${@} | grep -E -o -- '\<a[a-z]*\>')"

echo "PUSH_PULL: ${PUSH_PULL} "
echo "DELETE_OPT: ${DELETE_OPT} "
echo "SERVER_OPT: ${SERVER_OPT}"
if [[ -z "${SERVER_OPT}" ]]; then
	echo "The server name doesn't match the needed format"
	exit 2
fi

# push => source is the locam machine 
# pull => source is the remote server 


if [[ ! -z "${SERVER_OPT}" ]]; then
	SERVER_OPT="$(cat "${ARKCONF}" | grep -E '^WHERE="(a[a-z]*\s*)+"$' \ | 
		sed -E 's/^WHERE="(a[a-z]*\s*)+"$/\1/')"
fi

ABS_PATH_DIR="$(cat "${ARKCONF}" | grep -E '^WHAT="\/[a-zA-Z0-9_-/]*\/"$' \ | 
	sed -E 's/^WHAT="(\/[a-zA-Z0-9_-/]*\/)"$/\1/')"

USER="$(cat "${ARKCONF}" | grep -E '^WHO="[a-zA-Z0-9_-]+"$' \ | 
	sed -E 's/^WHO="[a-zA-Z0-9_-]+"$/\1/')"

# format the src and dest strings 
SRC_STR=''
DEST_STR=''

get_all_sources_and_dests(){

	if [[ "${PUSH_PULL}" == 'push' ]]; then
		# src is the local machine 
		SRC_STR="${ABS_PATH_DIR}"
		while read SERVER; do
			DEST_STR="${USER}@""${DEST_STR}""${ABS_PATH_DIR}""\n"
		done < <(echo "${SERVER_OPT}")
	else 
		# src is the local machine 
		DEST_STR="${ABS_PATH_DIR}"
		while read SERVER; do
			SRC_STR="${USER}@""${DEST_STR}""${ABS_PATH_DIR}""\n"
		done < <(echo "${SERVER_OPT}")
	fi
}

print_to_user(){
	for SRC in "${SRC_STR}";do
		for DEST in "${DEST_STR}"; do
			rsync -v -n "${SRC}" "${DEST}"
		done
	done
}

prompt_user(){
	echo "Do you wish to proceed? "
	read answer 
	if [[ "${answer}" =~ 'y|Y' ]]; then 
		exit 0
	fi 
	exit 1
}

do_sync(){
	if [[ -z "${DELETE_OPT}" ]]; then
		for SRC in "${SRC_STR}";do
			for DEST in "${DEST_STR}"; do
				rsync -a "${SRC}" "${DEST}"
			done
		done
	else 
		for SRC in "${SRC_STR}";do
			for DEST in "${DEST_STR}"; do
				rsync --delete "${SRC}" "${DEST}"
			done
		done
	fi
}

get_all_sources_and_dests
print_to_user
prompt_user
do_sync
