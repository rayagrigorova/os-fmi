#!/bin/bash

FQDN='[a-z0-9]([a-z0-9]+\.)*[a-z0-9]+\.'
TTL='([0-9]+)?'
CLASS='IN'
TYPE='SOA|NS|A|AAAA'
RDATA_COMMON='.*'
RDATA_SOA="${FQDN}\s*${FQDN}\s*([0-9]+\s*){5}"

SOA_ONE_LINE="^${FQDN}\s+${TTL}\s+${CLASS}\s+SOA\s+${RDATA_SOA}"
SOA_MULTILINE="^${FQDN}\s+${TTL}\s+${CLASS}\s+SOA\s+${FQDN}\s*${FQDN}\s*\s*\(\s*"'$'

change_TT(){
	CURRENT_DATE="$(date +%Y%m%d)"
	TT="$(bc < <(echo ${RDATA} | sed -E 's/^.*(..)$/\1/'))"

	# echo "${TT}"

	if [[ "${TT}" -ge 99 ]]; then
		echo "Uh oh! Big TTs"
		exit 1
	fi

	TT="$(( "${TT}" + 1))"
	if [[ "${TT}" -lt 10 ]]; then 
		TT="0""${TT}"
	fi
	
	if [[ "${CURRENT_DATE}" != "${RDATA}" ]]; then
		RDATA="${CURRENT_DATE}"'00'
	else 
		RDATA="$(echo "${RDATA}" | sed -e 's/^(.*)..$/\1/' )""${TT}"
	fi
# 	echo "${RDATA}"
}

for FILE in ${@}; do
	ONE_LINE="$(grep -E --color "${SOA_ONE_LINE}" "${FILE}" )"
	MULTILINE="$(grep -E --color "${SOA_MULTILINE}" "${FILE}")"
	LINE_TO_CHANGE=''
	RDATA=''

	if [[ ! -z "${ONE_LINE}" ]]; then
		LINE_TO_CHANGE="${ONE_LINE}"
# 		echo "ONE LINE "${ONE_LINE}" "
		RDATA=$(grep -E --color "${SOA_ONE_LINE}" "${FILE}" | cut -d ' ' -f 7)
# 		echo "ONE LINE DATA: ${ONE_LINE_RDATA}"

	else 
		LINE_TO_CHANGE="${MULTLINE}"
# 		echo "MULTI LINE "${MULTILINE}" "
		RDATA=$(grep -E -A 1 --color "${SOA_MULTILINE}" "${FILE}" |\
			tail -n 1 | grep -o -E '[0-9]+' )
# 		echo "MULTI LINE DATA: ${MULTILINE_RDATA}"
	fi
	change_TT
	echo "${RDATA}"
	grep -E --color "s/^(${FQDN}\s+${TTL}\s+${CLASS}\s+SOA\s+${FQDN}\s*${FQDN}\s*)([0-9]+\s*){1}(\s*[0-9]+\s*){4}/\1${RDATA}\3/" "${FILE}"
	if [[ ! -z "${ONE_LINE}" ]]; then
		sed -E -i "s/^(${FQDN}\s+${TTL}\s+${CLASS}\s+SOA\s+${FQDN}\s*${FQDN}\s*)([0-9]+\s*){1}(\s*[0-9]+\s*){4}/\1${RDATA}\3/"  "${FILE}" 
	else 
		sed -i -E "s/^(\s*)([^[:blank:]]+)(\s*)(;\s*serial\s*)$/\1${RDATA}\3\4/" "${FILE}"
	fi

done
