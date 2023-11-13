#!/bin/bash

# this is a crude test script that is used in the cmake to test if the decryption works
# it needs the boxcryptor pasword and keyfile to be set as environment variables,
# see below

if [-y "$BOXCRYPTOR_PASSWORD" ]; then
	echo "BOXCRYPTOR_PASSWORD environment variable is not set"
	exit 1
fi

if [-y "$BOXCRYPTOR_KEYFILE" ]; then
	echo "BOXCRYPTOR_KEYFILE environment variable is not set"
	exit 1
fi

file_path=$1
known_good_file=$2
password=$BOXCRYPTOR_PASSWORD
keyfile=$BOXCRYPTOR_KEYFILE
outfile=$(basename "$file_path" .bc)

if [ -z "$file_path" ]; then
	echo "No file path provided"
	exit 1
fi

if [ -z "$known_good_file" ]; then
	echo "No known good file provided"
	exit 1
fi

if [ -f "$outfile" ]; then
	rm "$outfile"
fi

# Your decryption command here, writing output to a file
./bc-decrypt "$BOXCRYPTOR_KEYFILE" "$1" "$password" "$outfile"

# Compare the output file with the known good file
if cmp "$known_good_file" "$outfile"; then
	echo "Decryption SUCCESSFUL"
	exit 0 # Return 0 on success
else
	echo "Decryption FAILED"
	exit 1 # Return 1 on error
fi
