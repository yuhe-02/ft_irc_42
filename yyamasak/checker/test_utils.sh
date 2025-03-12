LOG_FILE="output.log"

send_commands() {
  local COMMANDS=("$@")
  commands=$(printf "%s\r\n" "${COMMANDS[@]}")
  echo "===================================================================" | tee -a "$LOG_FILE"
  timeout 2 bash -c "{ printf '%b' \"$commands\"; sleep 1; } | nc $HOST $PORT" | while read -r RESPONSE; do
    echo "Response: $RESPONSE" | tee -a "$LOG_FILE"
  done
  if [[ $? -eq 124 ]]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S') timeout" | tee -a "$LOG_FILE"
  fi
  echo "===================================================================" | tee -a "$LOG_FILE"
}
