#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 folder"
    exit 1
fi

folder="$1"

if [ ! -d "$folder" ]; then
    echo "Error: '$folder' is not a directory"
    exit 1
fi

shopt -s nullglob

files=("$folder"/*)

regular_files=()
for f in "${files[@]}"; do
    if [ -f "$f" ]; then
        regular_files+=("$f")
    fi
done

if [ "${#regular_files[@]}" -eq 0 ]; then
    echo "Error: no files found in '$folder'"
    exit 1
fi

paste "${regular_files[@]}" | awk -F '\t' '{
    printf "%d", NR
    for (i = 1; i <= NF; i++)
        printf " & %s", $i
    print " \\\\"
}'
