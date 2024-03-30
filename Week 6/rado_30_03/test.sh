#!/bin/bash

A=3

#IF
if [[ $A -eq 3 ]]; then
	echo "A e ravno na 3"
fi

if echo '123' | grep -q -E '[0-9]*'; then
	echo "A e ravno na 3"
fi
	
# for cycles 
for i in 1 2 3; do
	echo ${i}
done

while true; do
	echo "while"
	break
done

# case 


