#!/usr/bin/env bash

set -euo pipefail

file="${1:-/dev/stdin}"

awk '
function elapsed_to_seconds(t, parts, n) {
    n = split(t, parts, ":")

    if (n == 3) {
        # h:m:s
        return parts[1] * 3600 + parts[2] * 60 + parts[3]
    }

    if (n == 2) {
        # m:s
        return parts[1] * 60 + parts[2]
    }

    # plain seconds, just in case
    return t
}

/^Running:/ {
    sample = $2
}

/^Elapsed:/ {
    elapsed = $2
    seconds = elapsed_to_seconds(elapsed)

    if (sample != "") {
        printf "%.2f\n", seconds
    } else {
        printf "%.2f\n", seconds
    }
}
' "$file"
