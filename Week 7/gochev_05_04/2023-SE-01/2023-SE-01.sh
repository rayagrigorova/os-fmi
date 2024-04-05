#!/bin/bash

# for (all files in a given dir)
# for (all forbidden words)
# replace with ** 

# checks: 2 args, the first one is a readable file, 2 is a dir 
# check if the bad words are 'words' by the definition given in the task
# ... 

while read FILE; do
	while read BAD_WORD; do
		# echo "file ${FILE} BAD_WORD ${BAD_WORD}"
		REPLACEMENT="$(echo  ${BAD_WORD} | tr '[a-zA-Z0-9_]' '*')"
		sed -i -E "s/\<${BAD_WORD}\>/${REPLACEMENT}/gi" "${FILE}"
	done < "${1}"
done < <(find "${2}" -type f -name '*.txt')
