#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PATH="$PATH:$W91_TOOLCHAIN_PATH/nds32le-elf-mculib-v5/bin"

binaryFile="$SCRIPT_DIR/output/telink_arduino_wifi_signed.bin"
demoObject="$SCRIPT_DIR/output/demo/demo_*.o"


rm -f "$binaryFile"
rm -f $demoObject

make

if [ -f "$binaryFile" ]; then
    "$SCRIPT_DIR/download.sh" "$binaryFile" "0x80080000"
else
    echo "Error: Binary file not found. Compilation may have failed."
    exit 1
fi

if [ $# -eq 1 ]; then
    if [ "$1" = "-log" ]; then
        minicom -D /dev/ttyUSB0 -b 115200
    fi
fi