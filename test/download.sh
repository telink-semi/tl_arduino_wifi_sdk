#!/bin/bash

# 1. 获取当前脚本所在的绝对目录（替代 %~dp0）
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 2. 拼接工具路径（Linux 使用正斜杠 /）
toolPath="$SCRIPT_DIR/../cli/tool"
downloadTool="$toolPath/sctool"      # 注意：Linux下通常没有 .exe 后缀
rambin="$toolPath/da/da.ram.bin"

# 3. 接收命令行参数（%1 -> $1, %2 -> $2）
binaryFile="$1"
port="/dev/ttyUSB0"                         # 提示：Linux 下的串口通常是 /dev/ttyUSB0 或 /dev/ttyACM0
speed="2000000"
addr="$2"

# 4. 打印开始日志
echo "-------------------Start Download[$binaryFile]-------------------"

# 5. 执行烧录命令
# 注意：如果工具没有执行权限，需先执行 chmod +x "$downloadTool"
"$downloadTool" -p "$port" -b "$speed" -da "$rambin" hw_reset 
"$downloadTool" -p "$port" -b "$speed" -da "$rambin" --after hw_reset flash_write "$addr" "$binaryFile"

# 6. 打印完成日志
echo "-----------------Download Complete[$binaryFile]-----------------"