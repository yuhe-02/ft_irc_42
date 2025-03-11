#!/bin/bash

# default
export HOST="127.0.0.1"
export PORT="8080"
export JSON_FILE="testcase1.json"

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

jq -c '.[]' "$JSON_FILE" | while read -r node; do
    title=$(echo "$node" | jq -r '.title')
    cmds=$(echo "$node" | jq -r '.cmds[]')
    echo "Executing: $title"
    command_list=()
    while IFS= read -r line; do
        command_list+=("$line")
    done <<< "$cmds"
    send_commands "${command_list[@]}"
done
