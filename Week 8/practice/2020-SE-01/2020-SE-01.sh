#!/bin/bash


if [[ "${#}" -ne 2 ]]; then
	echo 'Wrong argument number'
	exit 1
fi


if [[ ! -f "${1}" || ! -d "${2}" ]]; then
	echo 'Wrong argument format - the first arg should be a file and the second one should be a directory'
	exit 2
fi

if [[ ! -w "${1}" || ! -x "${2}" ]]; then
	echo 'You dont have permissions'
	exit 3
fi

echo 'hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Porst,license,SN,key' > "${1}"

while read FILENAME; do
	FILE_HOSTNAME="$(basename --suffix='.log' "${FILENAME}")"
# 	echo "${FILE_HOSTNAME}"
	echo -n "${FILE_HOSTNAME}," >> "${1}"
	while read LINE; do 
		if echo "${LINE}" | grep -E -q "^[[:blank:]]*([^[:blank:]]+[[:blank:]]*)+[[:blank:]]*:[[:blank:]]*([^[:blank:]]+[[:blank:]]*)+[[:blank:]]*$" ; then
			LOG_VALUE="$(echo "${LINE}" | sed -E 's/^[[:blank:]]*([^[:blank:]]+[[:blank:]]*)+[[:blank:]]*:[[:blank:]]*([^[:blank:]]+[[:blank:]]*)+[[:blank:]]*$/\2/')"
			echo "Log value: ${LOG_VALUE}"
			echo -n "${LOG_VALUE}," >> "${1}"
		elif echo "${LINE}" | grep -P -q "^\s*This platform has an?\s+.*\s+licence.\s*$"; then
			LICENCE="$(echo "${LINE}" | sed -E 's/^\s*This platform has an?\s+(.*)\s+licence.\s*$/\1/')"
			echo "Licence: ${LICENCE}"
			echo -n "${LICENCE}," >> "${1}"
		fi
	done < "${FILENAME}"
	echo >> "${1}"
done < <(find "${2}" -type f -name '*.log' )
sed -E -i 's/,$//' "${1}" 

