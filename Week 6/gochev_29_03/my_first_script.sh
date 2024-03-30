#!/bin/bash

# DONT ADD SPACES!!! 
my_var='think'
my_file='file.txt'
# cat "${my_file}"

# echo "Hello! I ${my_var} the time is:" 
# date
# 
# echo "Arg1: ${1}"
# echo "Arg2: ${2}"
# echo "Total count: ${#}
# echo "All arguments:  ${#}"

# read my_var
# echo "${my_var}"

# The variables remain empty because pipes are executed in subshells. Solution: process subsitutuion
stat --printf '%g %i\n' file.txt | read group inode
echo "Group: ${group}; Inode: ${inode}"

read group inode < <(stat --printf '%g %i\n' file.txt)
echo "Group: ${group}; Inode: ${inode}"

# if [['foo' != 'bar']]; then 
# 	echo shjksd
# fi
# 
# while command; do
# 
# done


while [[ "${i}" -le 5 ]]; do
	echo "${i}"
	i="$((i+1))"
done


while read a b ; do
	echo "a is ${a}"
	echo "b is ${b}"
done < text.txt

while read line ; do
	echo "line is ${line}"
done < text.txt

while read line ; do
	echo "line is ${line}"
done < <(fortune)
