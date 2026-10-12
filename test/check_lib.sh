#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")" && pwd)"
libList="$SCRIPT_DIR/lib_list.txt"
sdkDir="$SCRIPT_DIR/../sdk"

if [ ! -f "$libList" ]; then
    echo "Error: lib list not found: $libList"
    exit 1
fi

fail=0

while IFS=$'\t' read -r url relPath; do
    # skip blank lines and comments
    case "$url" in ""|\#*) continue;; esac

    dest="$sdkDir/$relPath"
    # the url basename carries the sha256 of the file, e.g. xxx.a-<hash>
    expectHash="$(basename "$url")"
    expectHash="${expectHash##*-}"

    # skip download if the local file already exists and matches the expected hash
    if [ -f "$dest" ] && [ "$(sha256sum "$dest" | awk '{print $1}')" = "$expectHash" ]; then
        echo "ok: $relPath"
        continue
    fi

    mkdir -p "$(dirname "$dest")"
    echo "download: $url"
    echo "      -> $dest"
    if ! curl -fL "$url" -o "$dest"; then
        echo "Error: failed to download $url"
        rm -f "$dest"
        fail=1
        continue
    fi

    if [ "$(sha256sum "$dest" | awk '{print $1}')" != "$expectHash" ]; then
        echo "Error: sha256 mismatch for $relPath"
        rm -f "$dest"
        fail=1
    fi
done < "$libList"

if [ "$fail" -ne 0 ]; then
    echo "Error: some libraries failed to download"
    exit 1
fi

echo "all libraries are up to date"