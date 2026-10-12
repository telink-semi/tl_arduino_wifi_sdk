/********************************************************************************************************
 * @file    core.c
 *
 * @brief   This is the source file for Arduino SDK
 *
 * @author  Platform Group
 * @date    2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "port_config.h"
#if BASED_ON_TELINK_WIFI_SDK

#include "port_interface.h"
#include "core.h"
#include "sdk_common.h"


#define FLASH_BASE_ADDR 0x80000000
#define D25_START_ADDR (FLASH_BASE_ADDR + 0x200000)
#define SYS_REG_BASE 0xf1700000
#define SMU_BASE 0xf0100000

#define REG_SYS_CLK_CTRL    (SYS_REG_BASE+0x214)
#define REG_CORE_RESET_CTRL (SYS_REG_BASE+0x218)
#define REG_D25_RST_VET     (SMU_BASE+0x200)
#define REG_N22_RST_VET     (SMU_BASE+0x204)

typedef union {
    uint16_t val;
    struct {
        uint16_t d25_div_num:3;
        uint16_t n22_div_num:3;
        uint16_t ahb_div_num:3;
        uint16_t apb_div_num:3;
        uint16_t root_clk_sel:1;
        uint16_t d25_clk_en:1;
        uint16_t n22_clk_en:1;
        uint16_t aon_apb_en:1;
    } bits;
}sys_clk_ctrl_t;

typedef union {
    uint8_t val;
    struct {
        uint8_t d25_sw:1;
        uint8_t :3;
        uint8_t n22_sw:1;
        uint8_t :3;
    } bits;
}core_reset_ctrl_t;

#define READ_REG8(addr)       (*(volatile uint8_t *)(addr))
#define READ_REG16(addr)       (*(volatile uint16_t *)(addr))
#define READ_REG32(addr)       (*(volatile uint32_t *)(addr))

#define WRITE_REG8(addr,val)   ((*(volatile uint8_t *)(addr)) = (val))
#define WRITE_REG16(addr,val)   ((*(volatile uint16_t *)(addr)) = (val))
#define WRITE_REG32(addr,val)   ((*(volatile uint32_t *)(addr)) = (val))

#define SHARED_LOG_BASE_ADDR  CONFIG_D25_COPROCESSOR_SHARED_LOG_BASE_ADDR
#define CHECK_OK_FLAG (0x55AA55AA)
#define CHECK_IPC_OK_FLAG (0xAA55AA55)
#define SHARED_LOG_BUFFER_SIZE CONFIG_D25_COPROCESSOR_SHARED_LOG_SIZE

#define reg_sys_clk_ctrl    READ_REG16(REG_SYS_CLK_CTRL)
#define reg_core_reset_ctrl READ_REG8(REG_CORE_RESET_CTRL)
#define reg_n22_rst_vet     READ_REG32(REG_N22_RST_VET)
#define reg_d25_rst_vet     READ_REG32(REG_D25_RST_VET)
#define barrier() 	__asm__ __volatile__("": : :"memory")
typedef struct {
    volatile uint32_t cur_pos_addr;
    volatile uint32_t buffer_size;
    uint32_t rsvd1;
    volatile uint32_t n22_rd_pos;
    uint32_t rsvd2;
    volatile uint32_t n22_wr_pos;
    volatile uint32_t wr_lock;
    volatile uint32_t d25_check_flag; 
    volatile uint8_t buffer[];
} d25_log_header_t;

typedef struct {
    d25_log_header_t *header;
    uint32_t time_ref;
    uint8_t *log_buffer;
    uint32_t log_buffer_size;
    bool init_flag;
} d25_log_ctx_t;


static d25_log_ctx_t d25_log_ctx = {0};
static void *flash_hw_lock_mutex = NULL;
static bool reset_d25_core_flag = false;

static inline void flash_hw_lock_init(void) {
    flash_hw_lock_mutex = create_mutex();
}

static void d25_log_init(void) {
    memset(&d25_log_ctx, 0, sizeof(d25_log_ctx_t));
    d25_log_ctx.header = (d25_log_header_t*)SHARED_LOG_BASE_ADDR;
    memset(d25_log_ctx.header, 0, sizeof(d25_log_header_t));
    
    // write current position address
    WRITE_REG32(&d25_log_ctx.header->cur_pos_addr, SHARED_LOG_BASE_ADDR);

    // write buffer size
    WRITE_REG32(&d25_log_ctx.header->buffer_size, SHARED_LOG_BUFFER_SIZE - sizeof(d25_log_header_t));
 
    // log_d("pos:%08x, buffer_size:%d, d25_check_flag:%d", d25_log_ctx.header->cur_pos_addr, d25_log_ctx.header->buffer_size, d25_log_ctx.header->d25_check_flag);
    d25_log_ctx.time_ref = millis();
    d25_log_ctx.log_buffer_size = SHARED_LOG_BUFFER_SIZE >> 1;
    d25_log_ctx.log_buffer = malloc(d25_log_ctx.log_buffer_size + 1);
    if(d25_log_ctx.log_buffer) {
        memset(d25_log_ctx.log_buffer, 0, d25_log_ctx.log_buffer_size);
    }
}

static bool d25_log_init_check(uint32_t timeout_ms) {
    uint32_t timeRef = millis();
    while(d25_log_ctx.header->d25_check_flag != CHECK_OK_FLAG) {
        delay_ms(1);
        if(millis() - timeRef > timeout_ms) {
            break;
        }
    }

    return (d25_log_ctx.header->d25_check_flag == CHECK_OK_FLAG);
}

static bool d25_ipc_init_check(uint32_t timeout_ms) {
    uint32_t timeRef = millis();
    while(d25_log_ctx.header->d25_check_flag != CHECK_IPC_OK_FLAG) {
        delay_ms(1);
        if(millis() - timeRef > timeout_ms) {
            break;
        }
    }

    return (d25_log_ctx.header->d25_check_flag == CHECK_IPC_OK_FLAG);
}

#define D25_LOG_START_STR    "[D25]"
static uint8_t * find_d25_log_start(uint8_t *buffer, uint32_t size) {
    uint32_t pos = 0;
    uint32_t str_len = strlen(D25_LOG_START_STR);
    while(pos + str_len <= size) {
        if(memcmp(buffer + pos, D25_LOG_START_STR, str_len) == 0) {
            return buffer + pos;
        }
        pos++;
    }
    return NULL;
}


static void d25_log_dump_handler(void * arg) {
    (void)arg;
    if(!reset_d25_core_flag) {
        return;
    }

    uint32_t rd_pos = d25_log_ctx.header->n22_rd_pos;
    uint32_t wr_pos = d25_log_ctx.header->n22_wr_pos;
    uint32_t data_size = wr_pos - rd_pos;

    if(data_size > d25_log_ctx.header->buffer_size) {
        data_size = d25_log_ctx.log_buffer_size - 1;
        rd_pos = wr_pos - data_size;
    }

    if(data_size > 0) {
        uint32_t rw_len;
        uint8_t *buffer = d25_log_ctx.log_buffer;
        uint8_t last_byte = 0;
        while(data_size && buffer) {
            rw_len = data_size > (d25_log_ctx.log_buffer_size - 1) ? (d25_log_ctx.log_buffer_size - 1) : data_size;
            for(uint32_t i = 0; i < rw_len; i++) {
                buffer[i] = d25_log_ctx.header->buffer[rd_pos % d25_log_ctx.header->buffer_size];
                rd_pos = rd_pos + 1;
            }
            data_size -= rw_len;
            buffer[rw_len] = '\0';
            last_byte = buffer[rw_len - 1];
            buffer = find_d25_log_start(buffer, rw_len);
            if(buffer) {
                log("%s", buffer);
            }
            port_yield();
        }
        d25_log_ctx.header->n22_rd_pos = rd_pos;

        if(last_byte != '\n') {
            log("\n");
        }
    }
}

static inline void start_d25(void) {
    // set d25 reset vector
    reg_d25_rst_vet = D25_START_ADDR + 0xa0;

    // reset d25 core
    core_reset_ctrl_t core_reset_ctrl;
    core_reset_ctrl.val = reg_core_reset_ctrl;
    core_reset_ctrl.bits.d25_sw = 1;
    reg_core_reset_ctrl = core_reset_ctrl.val;

    // sys_clk_ctrl_t sys_clk_ctrl;
    // sys_clk_ctrl.val = reg_sys_clk_ctrl;
    // log_d("Root Clk:%d, D25 Clk Div:%d, N22 Clk Div:%d", sys_clk_ctrl.bits.root_clk_sel, sys_clk_ctrl.bits.d25_div_num, sys_clk_ctrl.bits.n22_div_num);
}

void reset_d25_Core(void) {

    d25_log_init();
    start_d25();
    if(!d25_log_init_check(1000)) {
        log_e("d25_log_init_check timeout<1000ms>");
        return;
    }
    ipc_init();
    if(!d25_ipc_init_check(1000)) {
        log_e("d25_ipc_init_check timeout<1000ms>");
        return;
    }
    reset_d25_core_flag = true;
}

void create_d25_log_dump_task(void) {
    port_create_new_task(d25_log_dump_handler, NULL, 2048);
}


#define HEADER_STR  "#   "
#define REAR_STR    "   #"
#define PIN_STR     "Pin        "
#define FUNC_STR    "Function              "
#define ID_STR      "ID         "
#if 0
#define STATUS_STR  "Status   "
#else
#define STATUS_STR  ""
#endif
#define TITLE_STR   HEADER_STR PIN_STR FUNC_STR ID_STR STATUS_STR REAR_STR

#define PIN_LEN     strlen(PIN_STR)
#define FUNC_LEN    strlen(FUNC_STR)
#define ID_LEN      strlen(ID_STR)
#define STATUS_LEN  strlen(STATUS_STR)
#define BORDER_LEN  (PIN_LEN + FUNC_LEN + ID_LEN + STATUS_LEN + strlen(HEADER_STR) + strlen(REAR_STR))

static void show_device_info(void) {
    struct pinctrl_platform_pinmap *pinmap = pinctrl_get_platform_pinmap();
    char buffer[256];
    char *cur, *start;
    if(pinmap) {
        cur = buffer;
        while(cur - buffer < BORDER_LEN) {
            *cur++ = '#';
        }
        *cur = '\0';
        log("%s\n", buffer);

        log(TITLE_STR "\n");
        for(int i = 0; i < pinmap->nr_map; i++) {
            cur = buffer;

            start = cur;
            cur += sprintf(cur, "%d", pinmap->map[i].pin);
            while(cur - start < PIN_LEN) {
                *cur++ = ' ';
            }

            start = cur;
            cur += sprintf(cur, "%s", pinmap->map[i].func);
            while(cur - start < FUNC_LEN) {
                *cur++ = ' ';
            }

            start = cur;
            cur += sprintf(cur, "%s", pinmap->map[i].id);
            while(cur - start < ID_LEN) {
                *cur++ = ' ';
            }
        #if 0
            start = cur;
            cur += sprintf(cur, "%d", pinmap->map[i].status);
            while(cur - start < STATUS_LEN) {
                *cur++ = ' ';
            }
        #endif
            *cur++ = '\0';

            log(HEADER_STR "%s" REAR_STR "\n", buffer);
        }
        
        cur = buffer;
        while(cur - buffer < BORDER_LEN) {
            *cur++ = '#';
        }
        *cur = '\0';
        log("%s\n", buffer);
    }
}

void initVariant(void) {
    flash_hw_lock_init();
    if(1)
        show_device_info();
}

void flash_hw_lock(void) {
    lock_mutex(flash_hw_lock_mutex);
    if(PORT_CONFIG_USE_D25 && reset_d25_core_flag)
        ipc_driver_d25_wfi();
}

void flash_hw_unlock(void) {
    unlock_mutex(flash_hw_lock_mutex);
}

#define D25_STACK_SIZE (0x2000)
#define D25_STACK_START_ADDR (0xA022e000)
#define D25_STACK_VALUE (0x55555555)

static uint32_t d25_stack_free_size = D25_STACK_SIZE;
uint32_t d25_get_stack_free_size_low_level(void) {
    for (int i = 0; i < D25_STACK_SIZE; i += 4) {
        if (READ_REG32(D25_STACK_START_ADDR + i) != D25_STACK_VALUE) {
            d25_stack_free_size = i;
            break;
        }
    }
    return d25_stack_free_size;
}

#endif