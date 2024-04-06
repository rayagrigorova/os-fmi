#!/bin/bash

# Mask 022 => 666 becomes 644 

find ~ -type f -perm 644 -exec chmod g+w {} ';'
