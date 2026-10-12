@echo off
set cur_path=%~dp0
set input_file=%cur_path%root\
set output_file=%cur_path%littlefs.bin
set download_tool=%cur_path%\..\download.bat

if exist %output_file% (
    echo remove %output_file%
)

echo create %output_file
python -m littlefs create --block-size=4096 --block-count=64 %input_file% %output_file%

if exist %output_file% (
    echo download %output_file%
    %download_tool% %output_file% 0x80180000
)



