#!/bin/bash

find . -maxdepth 1 -type f -user "$(whoami)" -exec chmod 664 {} &>/dev/null ';' &>/dev/null
