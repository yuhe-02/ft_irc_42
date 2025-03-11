#!/bin/bash

LOG_FILE="output.log"
send_commands() {
  local COMMANDS=("$@")
  commands=$(printf "%s\r\n" "${COMMANDS[@]}")
  echo "===================================================================" | tee -a "$LOG_FILE"
  { printf "%b" "$commands"; sleep 1; } | nc $HOST $PORT | while read -r RESPONSE; do
    echo "Response: $RESPONSE" | tee -a "$LOG_FILE"
  done
  echo "===================================================================" | tee -a "$LOG_FILE"
}
