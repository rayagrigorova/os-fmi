#!/bin/bash

# checks: 1 argument that is a number
# for every user 
#   print total rss 
#   if total >> arg; kill process with highest rss 

if [[ "$(whoami)" != 'root' ]]; then 
	echo "This script should be executed by root"
	exit 1 
fi

for USER in $(ps -e -o user= | sort | uniq); do 
	TOTAL_RSS="$(ps -u "${USER}" -o rss= | awk '{sum + =$1} END {print sum}')"
	echo "User $USER total rss:" "${TOTAL_RSS}"
	
	if [[ "${TOTAL_RSS}" -gt "${1}" ]]; then 
		HIGHEST_PID="$(ps -u "${USER}" -o pid= --sort rss | tail -n 1)"
		kill "${HIGHEST_PID}"
		sleep 2 
		kill -KILL "${HIGHEST_PID}"
	fi
done
