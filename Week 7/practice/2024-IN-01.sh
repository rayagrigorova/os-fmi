#!/bin/bash

if [[ ! -f "{$ARKCONF}" ]]; then
	echo "The env variable is not set correctly"
	exit 1 
fi
