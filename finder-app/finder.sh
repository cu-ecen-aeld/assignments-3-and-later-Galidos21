#!/bin/sh
# Author: Bryan Basantes

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Error: Two arguments are required - a directory path and a search string."
    exit 1
fi

filesdir=$1
searchstr=$2

# Check if the provided directory path is valid
if [ ! -d "$filesdir" ]; then
    echo "Error: $filesdir is not a valid directory."
    exit 1
fi

# Count the number of files in the directory and subdirectories
file_count=$(find "$filesdir" -type f | wc -l)

# Count the number of matching lines across all files
match_count=$(grep -r "$searchstr" "$filesdir" 2>/dev/null | wc -l)

# Print the result
echo "The number of files are $file_count and the number of matching lines are $match_count"

# Exit with success code
exit 0
