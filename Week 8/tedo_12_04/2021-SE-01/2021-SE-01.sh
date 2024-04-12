#!/bin/bash

# Expectation: 
# user: oracle or grid 
# env var ORACLE HOME that contains a path to a directory 
# ORACLE_HOME/bin/adrci should exist and be executable 
# diag dest is a directory := /u01/app/<username>
# We know where to search for a command which, after being executed, outputs some lines

# 1. Check oracle or grid 
# 2. Check other expectations 
# 3. Get ADR home 
# du -sm 

if ! whoami | grep -qE '^(oracle|grid)$'; then
	echo 'This script should be executed by either oracle or grid' >&2
	exit 1
fi

if [[ -z "${ORACLE_HOME}" ]]; then
	echo 'HOME not set' >&2
	exit 2
fi

if [[ ! -d "${ORACLE_HOME}" ]]; then
	echo 'ORACLE_HOME must be a dir' >&2
	exit 3
fi

ADRCI="${ORACLE_HOME}/bin/adrci"
if [[ ! -x ""${ORACLE_HOME}"/bin/adrci" ]]; then
	echo '/bin/adrci not executable' >&2
	exit 4
fi

OUT="$("${ADRCI}" 'exec="show homes"')"
if [[ "${OUT}" == 'No ADR homes are set' ]]; then
	echo "${OUT}" >&2
	exit 5
fi

while read -r ADR_HOME; do
	du -sm  "/u0/app/$(whoami)/${ADR_HOME}"
done < <(echo "${OUT}" | tail -n +2 | sed -E 's/^\s*//')
