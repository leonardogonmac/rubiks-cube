#!/usr/bin/env bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 file_a file_b"
    exit 1
fi

file_a="$1"
file_b="$2"

paste "$file_a" "$file_b" | awk '
{
    if ($2 == 0) {
        print "division by zero on line " NR > "/dev/stderr"
        exit 1
    }

    print $1 / $2
}
'
