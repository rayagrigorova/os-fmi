#!/bin/bash

find / -type f -user "$(whoami)" 2>/dev/null | grep -E '\.blend[0-9]+$'
