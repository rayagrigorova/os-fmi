#!/bin/bash

echo "These are the files in your home dir"
find ~ -maxdepth 1 -mindepth 1 -printf "%f\n"
