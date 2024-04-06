#!/bin/bash

cat "${1}" | grep -E --color '^([^:]+:){4}[A-Za-z-]+\s+[a-zA-Z-]+a\s*[,:].*Inf.*$' |  \
cut -d ':' -f 1 | grep -E -o '^s[0-9]+'  | sed -E 's/^s[0-9]([0-9]{2})[0-9]*/\1/' | sort -n | uniq  -c | xargs | sort -nr | head -n 1 
