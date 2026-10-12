#!/bin/bash
cur_path=$(dirname "$(readlink -f "$0")")
input_file=$cur_path/root/
output_file=$cur_path/littlefs.bin
download_tool=$cur_path/../download.sh

if [ -f "$output_file" ]; then
    echo remove $output_file
fi

echo create $output_file
python3 -m littlefs create --block-size=4096 --block-count=64 $input_file $output_file

if [ -f "$output_file" ]; then
    echo download $output_file
    $download_tool $output_file 0x80180000
fi



