#!/bin/bash
grep --color=auto -E "^\w+[^ ](.+)[^;{]$" "$1"
