#!/bin/bash

TEMP_FILE="$(mktemp)"
echo 'aaaa' > "${TEMP_FILE}"
cat "${TEMP_FILE}"
