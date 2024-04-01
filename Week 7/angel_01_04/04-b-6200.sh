#!/bin/bash

# LOGGED_USERS="$(who | awk '{print $1}' | sort | uniq)"
# echo "${LOGGED_USERS}"
# USERS_WITH_LIVE_PROCESSES="$(ps -e -o euser= | sort | uniq)"
# echo "${USERS_WITH_LIVE_PROCESSES}"

comm -1 -3 <(who | awk '{print $1}' | sort | uniq) <(ps -e -o euser= | sort | uniq)
