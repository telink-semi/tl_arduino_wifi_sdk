#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")" && pwd)"
buildDir=$SCRIPT_DIR/output


if [ -d "$buildDir" ]; then
    rm -rf "$buildDir"
fi