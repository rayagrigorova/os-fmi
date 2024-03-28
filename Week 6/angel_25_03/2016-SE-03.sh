#!/bin/bash 

users="$(cat /etc/passwd | cut -d ':' -f 1)"

for name in ${users}; do
	homedir="$(cat /etc/passwd | grep -E "^${name}:" | cut -d ':' -f 6 )"
	# echo ${homedir}
	# echo "${dirowner}"

	if [[ ! -e "${homedir}" ]]; then
		echo "${name} has a non-existent directory"	
		continue
	fi

	dirowner="$(stat "${homedir}" -c "%U")"
	if [[ "${name}" != "${dirowner}" ]]; then
		echo "${name} is not the owner"	
	fi
done


