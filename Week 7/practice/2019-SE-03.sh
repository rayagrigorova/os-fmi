#!/bin/bash

if [[ ! -x '/home/velin' ]]; then
	echo "Permission deined for velin's home dir"
	exit 1
fi


# 1. Find the most recently changed file's inode 
MOST_RECENT_INODE="$(find '/home/velin' -type f -printf '%C@ %i\n' 2>/dev/null | sort -n -r | head -n 1 | cut -d ' ' -f 2 )"
DEPTH="$(find '/home/velin' -type f -inum ${MOST_RECENT_INODE} -printf '%d\n' 2>/dev/null )"
echo "${DEPTH}"
