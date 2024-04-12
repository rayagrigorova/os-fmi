#!/bin/bash

# Validate args - check if it's a directory and has execute rights

if [[ "${#}" -ne 1 ]]; then
	echo 'Wrong argument count'
	exit 1
fi

if [[ ! -d "${1}" ]]; then
	echo 'The argument must be a directory'
	exit 2
fi

if [[ ! -x "${1}" ]]; then
	echo 'You dont have execute rights for the directory'
	exit 3
fi
# Create a temp file that contains records in the following format:
# <word> <word_count>
# <word> <word_count>
# <word> <word_count>

STOP_WORDS="$(mktemp)"
MIN_COUNT=3
FILES_COUNT="$(( $(find "${1}" -type f | wc -l) / 2 ))"

# 1. Identify stop words - this will be done with a file containing records line <n> <word>, where n is the number of repetions of <word> 
# in some file and n >= 3. Then I will use awk to count the number of lines for each word and then output the words 
while read FILENAME; do
	# Words that have at least 3 reps in a file 
	LINE="$(grep -E -o '\<[a-z]+\>' "${FILENAME}" | sort \
		| uniq -c | sed -E 's/^[[:blank:]]+//' | awk '$1 >= 3 {print $0}')"
	if [[ -n "${LINE}" ]]; then
		echo "${LINE}" >> "${STOP_WORDS}"
	fi
done < <(find "${1}" -type f )


# 2. Identify non stop words that are good candidates (have a high repetition number)

GOOD_CANDIDATES="$(mktemp)"
while read FILENAME; do
	while read  CANDIDATE; do
			if grep -E -q "$(echo ${CANDIDATE} | cut -d ' ' -f 2 )" "${STOP_WORDS}"; then
				continue
			fi
			echo "${CANDIDATE}" >> "${GOOD_CANDIDATES}"
	done < <(grep -E -o '\<[a-z]+\>' ${FILENAME} | sort \
				| uniq -c | sed -E 's/^[[:blank:]]+//')
done < <(find "${1}" -type f )

cat <(cat "${STOP_WORDS}" | cut -d ' ' -f 2 | sort | uniq -c | sed -E 's/^[[:blank:]]+//' |  awk -v count="${FILES_COUNT}" '$1 >= count {print $2}') <(cat "${GOOD_CANDIDATES}" | awk '{arr[$2]+=$1} END{for (key in arr) print arr[key],key}' | sort -nr | cut -d ' ' -f 2 ) | head -n 10

rm -r "${STOP_WORDS}"
rm -r "${GOOD_CANDIDATES}"
