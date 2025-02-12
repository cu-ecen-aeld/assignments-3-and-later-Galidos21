#!/bin/sh
# Author: Bryan Basantes

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Error: Two arguments are required - a file path and a text string."
    exit 1
fi

writefile=$1
writestr=$2

# Create the directory path if it doesn't exist
directory=$(dirname "$writefile")

if [ ! -d "$directory" ]; then
    mkdir -p "$directory"
    if [ $? -ne 0 ]; then
        echo "Error: Could not create directory $directory."
        exit 1
    fi
fi

# Write the string to the file (overwrite if it exists)
echo "$writestr" > "$writefile"
if [ $? -ne 0 ]; then
    echo "Error: Could not write to file $writefile."
    exit 1
fi

echo "Successfully wrote to file $writefile."

# Exit with success code
exit 0
