#!/bin/bash

users="$(cat /etc/passwd | cut -d ':' -f 1)"

for name in ${users}; do

	homedir="$(cat /etc/passwd | grep -E "^${name}:" | cut -d ':' -f 6)"


	if [[ ! -e "${homedir}" ]]; then
		echo "${name} has a non-existant home dir"
		continue
	fi

	dirowner="$(stat "${homedir}" -c "%U")"

	if [[ "${name}" != "${dirowner}" ]]; then
		echo "${name} is not the owner of their directory±"
	fi
done
