#!/bin/bash

# default
export HOST="127.0.0.1"
export PORT="6667"

# parse options
while getopts "p:c:" opt; do
  case "$opt" in
    p) PORT="$OPTARG" ;;
    c) HOST="$OPTARG" ;;
    *) echo "Usage: $0 [-p port] [-c ip]" >&2; exit 1 ;;
  esac
done

# read source file
if [ "${BASH_SOURCE[0]}" == "$0" ]; then
	source ./test_utils.sh
fi


# Test 1
commands=(
  "PASS test"
  "NICK testuser"
  "USER testuser 0 * :Test User"
  "JOIN #testchannel"
  "QUIT"
)
send_commands "${commands[@]}"

# Test 2
commands1=(
  "NICK testuser"
  "USER testuser 0 * :Test User"
  "JOIN #testchannel"
  "QUIT"
)
send_commands "${commands1[@]}"