#!/bin/bash

# 1. Validation

if [[ "${#}" -ne 1 ]]; then
	echo "The number of arguments is wrong" >&2
	exit 1
fi

if [[ ! -d "${1}" ]]; then
	echo "The arg must be a directory" >&2
	exit 2
fi

if [[ ! -r "${1}" ]] || [[ ! -x "${1}" ]]; then
	echo "HEHEHEHEH" >&2
	exit 3
fi

# 2. Find all the the files in fuga/cfg that have 
# the given name format. Iterate through them
# 3. For each one,pass it to the validate.sh 
# script and look at the exit code. If the exit status is 
# 1, then print the stdout of validate.sh starting with <
# conf_file_name>: (I will do this using sed)
# 4. Dump the valid files into a temp file and then rewrite foo.conf
# 5. If the file name of a valid config file doesnt existr 
# as a username, generate a password and add its hashed version to foo.pwd
# 6. I will keeep a temp file for the output of the 
# script and sort it before catting it so that all errors and new 
# user names + passwords appear one after another

> "${1}/foo.conf" # Empty foo.conf since it will be ooverwritten anyway
FINAL_OUTPUT="$(mktemp)"
while read CONFIG_FILE; do
	COMM_OUT="$(mktemp)"
	"${1}/validate.sh" "${CONFIG_FILE}" >"${COMM_OUT}"
	RESULT="${?}"
	if [[ "${RESULT}" -eq 2 ]]; then
		continue
	elif [[ "${RESULT}" -eq 1 ]]; then 
		# Write the error to the final output file
		cat "${COMM_OUT}" >> "${FINAL_OUTPUT}"
	else
		# Valid file => add its contents to foo.conf
# 		echo 'Adding as valid'
		cat "${CONFIG_FILE}" >> "${1}/foo.conf"
		# User not found in pwd file => add him
		if ! grep -q -E "^$(basename --suffix='.cfg' ${CONFIG_FILE})" "${1}/foo.pwd"; then
			NEW_PASSWD="$(pwgen -N 1)"
			HASHED_PASSWD="$( echo ${NEW_PASSWD} | xargs mkpasswd )"
			echo "User: $(basename --suffix='.cfg' ${CONFIG_FILE}) password: ${NEW_PASSWD}" >> "${FINAL_OUTPUT}"
			echo "$(basename --suffix='.cfg' ${CONFIG_FILE}):${HASHED_PASSWD}" >> "${1}/foo.pwd"
		fi
	fi
	rm -r "${COMM_OUT}"
done < <(find "${1}" -type f -name '*.cfg')

cat "${FINAL_OUTPUT}" | sort 
rm -r "${FINAL_OUTPUT}"
