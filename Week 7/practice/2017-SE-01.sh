#!/bin/bash

find . -type f -exec stat -c '%h %n' {} ';' | sort -n -r  | head -n 5 | cut -d ' ' -f 2 
