MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))

SRC_DIR = ../sdk
TEST_DIR = .
DEMO_DIR = $(TEST_DIR)/demo
VARIANTS_DIR = $(SRC_DIR)/variants/w91
CORES_DIR = $(SRC_DIR)/cores/arduino
PORTS_DIR = $(CORES_DIR)/ports
BASE_SDK_DIR = $(PORTS_DIR)/telink_wifi_sdk
LIBRARIES_DIR = $(SRC_DIR)/libraries
TOOLS_DIR = $(TEST_DIR)/../cli/tool
OUT_DIR = $(TEST_DIR)/output

ifeq ($(OS),Windows_NT)
CONFIG_TOOLCHAIN_PATH = $(W91_TOOLCHAIN_PATH)/toolchains/nds32le-elf-mculib-v5/bin
clean_cmd = $(TEST_DIR)/clean.bat
else
CONFIG_TOOLCHAIN_PATH = $(W91_TOOLCHAIN_PATH)/nds32le-elf-mculib-v5/bin
clean_cmd = $(TEST_DIR)/clean.sh
endif

CROSS_COMPILE := $(subst $/",,$(CONFIG_TOOLCHAIN_PATH))/riscv32-elf-

CC     	 = $(CROSS_COMPILE)gcc
CPP      = $(CC) -E
CXX      = $(CROSS_COMPILE)g++
AS       = $(CROSS_COMPILE)as
LD       = $(CROSS_COMPILE)ld
AR       = $(CROSS_COMPILE)ar
NM       = $(CROSS_COMPILE)nm
LDR      = $(CROSS_COMPILE)ldr
STRIP  	 = $(CROSS_COMPILE)strip
OBJCOPY  = $(CROSS_COMPILE)objcopy
OBJDUMP  = $(CROSS_COMPILE)objdump
IMGTOOL = $(TOOLS_DIR)/imgtool

INCLUDE_DIRS = $(wildcard $(LIBRARIES_DIR)/*/src)


INCLUDES_BASE_SDK = -I $(BASE_SDK_DIR)/lib -I $(BASE_SDK_DIR)/include -I $(BASE_SDK_DIR)/api/include -I $(BASE_SDK_DIR)/include/FreeRTOS -I $(BASE_SDK_DIR)/include/freebsd -I $(BASE_SDK_DIR)/include/hal -I $(BASE_SDK_DIR)/include/hal/ndsv5 -I $(BASE_SDK_DIR)/lib/lwip/src/include -I $(BASE_SDK_DIR)/lib/lwip/ports/freertos/include -I $(BASE_SDK_DIR)/lib/hostap/utils -I $(BASE_SDK_DIR)/lib/scdc -I $(BASE_SDK_DIR)/lib/net/mqtt/coreMQTT -I $(BASE_SDK_DIR)/lib/net/mqtt/coreMQTT-agent -I $(BASE_SDK_DIR)/lib/net/esp-tls -I $(BASE_SDK_DIR)/lib/net/http -I $(BASE_SDK_DIR)/lib/net/esp_http_client/include -I $(BASE_SDK_DIR)/lib/tlsr_mcuboot/mcuboot/boot/espressif/hal/include/esp32 -I $(BASE_SDK_DIR)/lib/net/tcp-transport -I $(BASE_SDK_DIR)/lib/mbedtls-2.16.2/include/mbedtls -I $(BASE_SDK_DIR)/lib/ble/nimble/mynewt-core/encoding/base64/include
INCLUDES_BASE_SDK += -I $(BASE_SDK_DIR) -I $(BASE_SDK_DIR)/third_party/littlefs -I $(BASE_SDK_DIR)/include/cli -I $(BASE_SDK_DIR)/include/mbedtls 
INCLUDES_LIBRARIES = $(foreach dir, $(INCLUDE_DIRS), -I $(dir))

INCLUDES = -I $(VARIANTS_DIR) -I $(CORES_DIR) -I $(CORES_DIR)/util -I $(PORTS_DIR)
INCLUDES += $(INCLUDES_BASE_SDK) $(INCLUDES_LIBRARIES)

LIBS_RISCV = -L $(CONFIG_TOOLCHAIN_PATH)/../lib/gcc/riscv32-elf/10.3.0/../../../../riscv32-elf/lib -lc -L $(CONFIG_TOOLCHAIN_PATH)/../lib/gcc/riscv32-elf/10.3.0/../../../../riscv32-elf/lib -lm -L $(CONFIG_TOOLCHAIN_PATH)/../lib/gcc/riscv32-elf/10.3.0 -lgcc  

LIBS_COMMON += $(BASE_SDK_DIR)/lib/telink_wifi_sdk_w91.a 
LIBS = $(LIBS_RISCV) -lgcc -lstdc++ 

MACRO_DEFS = -D__WITS__ -DCONFIG_TEST_AUDIO_HTTP
CFLAGS = $(MACRO_DEFS) -include $(BASE_SDK_DIR)/include/generated/autoconf.h -include $(BASE_SDK_DIR)/include/rom/fixedconf.h -include $(BASE_SDK_DIR)/include/generated/kconfig2lwipopt.h -include $(BASE_SDK_DIR)/include/generated/kconfig2freertosopt.h $(INCLUDES) -Wall -Werror -fno-common -fno-exceptions -ffunction-sections -fdata-sections -pipe -mcmodel=medany -Wall -fno-strict-aliasing -fno-builtin -funroll-loops -ffunction-sections -fdata-sections -march=rv32imac -mabi=ilp32 -matomic -Os -g3
CPPFLAGS = $(CFLAGS)


C_FILES += $(wildcard $(CORES_DIR)/*.c)
C_FILES += $(wildcard $(VARIANTS_DIR)/*.c)
C_FILES += $(wildcard $(PORTS_DIR)/*.c)
C_FILES += $(wildcard $(PORTS_DIR)/*/*.c)
C_FILES += $(wildcard $(PORTS_DIR)/third_party/*/*.c)
C_FILES += $(wildcard $(BASE_SDK_DIR)/*.c)

CPP_FILES += $(wildcard $(CORES_DIR)/*.cpp)
CPP_FILES += $(wildcard $(LIBRARIES_DIR)/*/src/*.cpp)
CPP_FILES += $(wildcard $(DEMO_DIR)/*.cpp)


C_OBJS = $(filter-out %.c, $(patsubst $(CORES_DIR)/%.c, $(OUT_DIR)/cores/%.o, $(C_FILES))) \
         $(filter-out %.c, $(patsubst $(VARIANTS_DIR)/%.c, $(OUT_DIR)/variants/%.o, $(C_FILES))) 


CPP_OBJS = $(filter-out %.cpp, $(patsubst $(CORES_DIR)/%.cpp, $(OUT_DIR)/cores/%.o, $(CPP_FILES))) \
           $(filter-out %.cpp, $(patsubst $(LIBRARIES_DIR)/%.cpp, $(OUT_DIR)/libraries/%.o, $(CPP_FILES))) \
           $(filter-out %.cpp, $(patsubst $(DEMO_DIR)/%.cpp, $(OUT_DIR)/demo/%.o, $(CPP_FILES)))

OBJS = $(sort $(C_OBJS) $(CPP_OBJS))

_force = #FORCE

TARGET = $(OUT_DIR)/telink_arduino_wifi.elf
LDFLAGS = -T $(BASE_SDK_DIR)/link/wits.lds


$(OUT_DIR)/cores/%.o: $(CORES_DIR)/%.c  $(_force)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/variants/%.o: $(VARIANTS_DIR)/%.c  $(_force)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/demo/%.o: $(DEMO_DIR)/%.cpp  $(_force)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $<

$(OUT_DIR)/cores/%.o: $(CORES_DIR)/%.cpp  $(_force)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $<

$(OUT_DIR)/libraries/%.o: $(LIBRARIES_DIR)/%.cpp  $(_force)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $<


all: $(OBJS)
	$(LD) $(LDFLAGS) -T $(BASE_SDK_DIR)/link/wits.rom_v2.ld  --no-relax -Bstatic --gc-sections --no-dynamic-linker -o $(OUT_DIR)/telink_arduino_wifi.elf --start-group --whole-archive $(LIBS_COMMON) $(OBJS) --no-whole-archive $(LIBS) --end-group --print-memory-usage
	$(OBJCOPY) -O binary $(OUT_DIR)/telink_arduino_wifi.elf $(OUT_DIR)/telink_arduino_wifi.bin
	$(OBJDUMP) -t $(OUT_DIR)/telink_arduino_wifi.elf > $(OUT_DIR)/telink_arduino_wifi.txt
	$(IMGTOOL) sign --align 4 --version 0.1 --header-size 0x20 --slot-size 0x180000 --pad-header $(OUT_DIR)/telink_arduino_wifi.bin $(OUT_DIR)/telink_arduino_wifi_signed.bin

clean:
	@$(clean_cmd)


FORCE: