#!/bin/bash

TEMP_FILENAME=$(mktemp)
ps -e -o tty=,pid=,comm= | grep -E '^\?\s*[^\s]' | sed -E 's/^\?\s*([^\s].*)/\1/' > TEMP_FILENAME

echo "PID and commands for processes without tty:"
cat TEMP_FILENAME

echo "Unique commands for processes without tty:"
cat TEMP_FILENAME | awk '{print $2}' | sort | uniq
rm -f "${TEMP_FILENAME}"
