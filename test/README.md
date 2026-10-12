# Usage
## Install Toolchain
### Linux
```shell
# 1. Download Toolchain
curl -O https://doc.telink-semi.cn/tools/vsc/linux/toolchains/V511_Linux.tar.xz

# 2. Unpack Toolchain
sudo mkdir -p /opt
sudo tar -xvJf ./V511_Linux.tar.xz -C /opt/ --strip-components=2

# 3. Set Environment Variables
echo "export W91_TOOLCHAIN_PATH=/opt:$PATH" >> ~/.bashrc

# 4. Restart Terminal
```
### Windows
```shell
# 1. Download Toolchain
curl -O https://doc.telink-semi.cn/tools/vsc/windows/toolchains/V511_Windows.tar.xz

# 2. Unpack Toolchain
mkdir "D:\.telink" # this is a example path, you can change it to any path you like
tar -xf V511_Windows.tar.xz -C "D:\.telink"

# 3. Set Environment Variables
setx W91_TOOLCHAIN_PATH "D:\.telink\V511_Windows"
setx PATH "%W91_TOOLCHAIN_PATH%\toolchains\cygwin\bin;%PATH%"

# 4. Restart Terminal
```

## Download Bootloader
```shell
# assume that you are in the same directory as the README.md file

# Linux
chmod +x ./download.sh
./download.sh wits.tlsrboot.bin 0x80000000

# Windows
download.bat wits.tlsrboot.bin 0x80000000

```
## Update Libraries
```shell
# assume that you are in the same directory as the README.md file

# Linux
chmod +x ./check_lib.sh
./check_lib.sh
# Windows
check_lib.bat
```

## Compile and Download Arduino Firmware

```shell
# assume that you are in the same directory as the README.md file

# 1. Compile Firmware
make
# 2. Download Firmware
# Linux
./download.sh output/telink_arduino_wifi.bin 0x80080000
# Windows
download.bat output\telink_arduino_wifi.bin 0x80080000


# Compile and Download 
# Linux
chmod +x ./compile.sh
./compile.sh
# Windows
compile.bat

```


## Download D25 Firmware
```shell
# assume that you are in the same directory as the README.md file

# Linux
chmod +x ./download.sh
./download.sh d25_coprocessor.bin 0x80200000
# Windows
download.bat d25_coprocessor.bin 0x80200000

```