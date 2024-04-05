#!/bin/bash

for i in $(seq 1 10); do
	echo "${i}"
done

for i in "{$@}"; do
	echo '['"${i}"']'
done


for i in "{$*}"; do
	echo '['"${i}"']'
done

hello(){
	echo "Function parameter: {$1}"
}

hello "Ivan Ivan"

# The b variable doesn't contain anything because piping leads to execution in a subshell 
# In the subshell the variables have 'one' 'two' and 'three' as contents
echo "one two three" | read a b c
echo $b

# This works
read a b c <  <(echo "one two three")
echo $b

echo "one two three" | (read a b c; echo $c)

FILE=$(mktemp)
# Do something here ... 
rm "${FILE}"

for i  in 1 2 3; do
	echo "${i}"
done 
