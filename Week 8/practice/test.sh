#/bin/bash 

echo 'ASTERISK'
for ARG in "${@}"; do
	echo "${ARG}"
done
echo 'STAR'
for ARG in "${*}"; do
	echo "${ARG}"
done
