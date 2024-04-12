#!/bin/bash

# The srcipt acts like the old java but actually calls the new java

# The expected format of the argumets
# given to the script is [optopns] -jar filename [args]
# In the new implementation everyting after the jar file is an arg
# (like in the standard version). Dproperty=value should be moved after -jar 

# This script will simply print the line of code that should be executed 
# (java .... )
ALL_ARGS="${*}"
# echo "${ALL_ARGS}"

# The arguments (if there are any) are where they are supposed to be
# 



