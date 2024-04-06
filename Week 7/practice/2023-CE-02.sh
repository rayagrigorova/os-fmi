#!/bin/bash

# Find the valid log files 

find /var/log/my_logs -type f -regextype posix-extended -regex '[a-zA-Z0-9_]+_[0-9]+\.log$' -print0 | xargs -0 cat | grep -o -E -i '\<error\>' | wc -l


