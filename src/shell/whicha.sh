#!/bin/bash

prog=$1

if [[ $# != 1 ]]; then
    echo "Usage: $0 <program>"
    exit
fi

for p in $(echo $PATH | tr ':' ' '); do
    if [[ -x "$p/$prog" && ! -d "$p/$prog" ]]; then
        echo $p/$prog
    fi
done
