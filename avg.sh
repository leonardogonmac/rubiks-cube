#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 file"
    exit 1
fi

file="$1"

if [ ! -f "$file" ]; then
    echo "Error: '$file' is not a file"
    exit 1
fi

awk '
{
    sum += $1
    count++
}
END {
    if (count == 0) {
        print "Error: file is empty"
        exit 1
    }

    print sum / count
}
' "$file"
