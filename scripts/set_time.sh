#!/bin/bash
# Sync the RTC to the local wall-clock time.
# Opens the serial port once (one DTR reset), listens for boot messages,
# sends T<epoch>\n during the firmware's listen window, and captures the
# Arduino's response so we can see what happened.
#
# Usage: scripts/set_time.sh /dev/cu.usbmodem2101
set -euo pipefail

PORT="${1:-}"
if [ -z "$PORT" ]; then
  echo "ERROR: no port provided" >&2
  exit 1
fi
if [ ! -e "$PORT" ]; then
  echo "ERROR: $PORT does not exist" >&2
  exit 1
fi

LOCAL_EPOCH=$(python3 -c "import time; print(int(time.time() - time.timezone + (3600 if time.daylight and time.localtime().tm_isdst else 0)))")

# stty -f and exec 3<> each open the port (each causes a DTR reset). What
# matters is that after fd 3 is held open, no further opens happen, so the
# Arduino's listen window runs uninterrupted. Run stty first, then open fd 3.
stty -f "$PORT" 9600 raw -echo -echoe -echok

# Open the port on fd 3 and hold it open for the rest of the script — no
# further resets after this point.
exec 3<>"$PORT"

# Background reader: print anything the Arduino says, prefixed for clarity.
( while IFS= read -r line <&3; do echo "[arduino] $line"; done ) &
READER_PID=$!

# Cleanup the reader on exit no matter what.
trap 'kill $READER_PID 2>/dev/null || true; exec 3>&- 2>/dev/null || true' EXIT

# Wait for Arduino bootloader + setup() to reach its listen window.
sleep 3

echo "[host]    sending T$LOCAL_EPOCH"
printf 'T%s\n' "$LOCAL_EPOCH" >&3

# Keep listening briefly so we see the sync confirmation.
sleep 3
