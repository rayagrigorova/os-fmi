#!/bin/bash

# To get a package archive, tar and xz the tree dir 
# First arg - path to the repo
# Second arg - path to the package 

if [[ "${#}" -ne 2 ]]; then
	echo 'The number of args should be 2'
	exit 1
fi


if [[ ! -d "${1}" || ! -d "${2}" ]]; then
	echo "Both args should be directories"
	exit 2
fi

# First, check if the same version of the package exists 
PACKAGE_VERSION="$(cat "${2}/version")"
# echo "${PACKAGE_VERSION}"
PACKAGE_NAME="$(dirname "${2}/version")"
# echo "${PACKAGE_NAME}"
if grep -P "^${PACKAGE_NAME}\s*-\s*${PACKAGE_VERSION}\s+" "${1}/db" ; then # The same version exists 
	# Copy the tree directory into the repository under <repo_name>/packages
	cp -r "${2}/tree" "${1}/packages/${PACKAGE_NAME}"
	tar -cf "${1}/packages/${PACKAGE_NAME}.tar" "${1}/packages/${PACKAGE_NAME}"
	# Calculate hash 
	SHA="$(sha256sum "${1}/packages/${PACKAGE_NAME}.tar" | cut -d ' ' -f 1 )"
# 	echo "${SHA}"
	# Change the name of the archive to the hash sum
	mv "${1}/packages/${PACKAGE_NAME}.tar" "${1}/packages/${SHA}.tar"  
	# Compress 
	xz -z "${1}/packages/${SHA}.tar"  
	# Update the database 
	sed -i -E "s/^(${PACKAGE_NAME}\s*-\s*${PACKAGE_VERSION}\s+)[0-9a-zA-Z]+\s*$/\1"${SHA}/"" "${1}/db" 
	sort -o "${1}/db" "${1}/db" 
else 
	# Copy the tree directory into the repository under <repo_name>/packages
	cp -r "${2}/tree" "${1}/packages/${PACKAGE_NAME}"
	tar -cf "${1}/packages/${PACKAGE_NAME}.tar" "${1}/packages/${PACKAGE_NAME}" 
	rm -r "${1}/packages/${PACKAGE_NAME}"
	SHA="$(sha256sum "${1}/packages/${PACKAGE_NAME}.tar" | cut -d ' ' -f 1 )"
# 	echo "${SHA}"
	# Change the name of the archive to the hash sum
	mv "${1}/packages/${PACKAGE_NAME}.tar" "${1}/packages/${SHA}.tar"  
	# Compress 
	xz -z "${1}/packages/${SHA}.tar"  
	# Update the database 
	echo "${PACKAGE_NAME}-${PACKAGE_VERSION} ${SHA}" >> "${1}/db" 
	sort -o "${1}/db" "${1}/db" 
fi
