#!/bin/bash

# Directory where files will be created
directory="test_files"

# Create directory if it doesn't exist
mkdir -p "$directory"

# Function to create a file with specified content
create_file() {
    echo "$2" > "$directory/test_file_$1.txt"
}

# Function to concatenate files with empty lines between
combine_files() {
    start=$1
    end=$2
    combined_file=$3

    > "$directory/$combined_file" # Create an empty file to start with
    for i in $(seq $start $end); do
        cat "$directory/test_file_$i.txt" >> "$directory/$combined_file"
        echo "" >> "$directory/$combined_file" # Add an empty line
    done
}

# Contents for the first 5 files
contents=("aa" "bb" "cc" "dd" "ee")

# Create first 5 files and their combined file
for i in {1..5}; do
    create_file $i ${contents[$i-1]}
done
combine_files 1 5 "combined_file_1.txt"

# Contents for the next 5 files
contents=("ff" "gg" "hh" "ii" "jj")

# Create next 5 files and their combined file
for i in {6..10}; do
    create_file $i ${contents[$i-6]}
done
combine_files 6 10 "combined_file_2.txt"

echo "Test files and combined files created in $directory/"
