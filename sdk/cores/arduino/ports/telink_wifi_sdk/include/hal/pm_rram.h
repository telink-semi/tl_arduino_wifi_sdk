/*******************************************************************************************************
 *
 * @file    pm_rram.h
 *
 * @brief   This is the source file for TLSR9118
 *
 * @author  
 * @par     Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * @author  
 * @par     Copyright 2023-2025 Senscomm Semiconductor Co., Ltd.
 * 
 * @author  Telink WIFI GROUP
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 
#ifndef _PM_RRAM_H_
#define _PM_RRAM_H_

#ifdef CONFIG_PM_MULTI_CORE
#define TLSR9118_PM_RRAM_SIG_DATA            __pmsigram_start
#endif

/* share memory address */
#define TLSR9118_PM_RRAM_DATA_ADDR           __pmrram_start

/* firmware address */
#define TLSR9118_PM_RRAM_FW_ADDR             (u32)__watcher_start

/* pm signalling data */
#define TLSR9118_PM_RRAM_INFO_ADDR           (TLSR9118_PM_RRAM_DATA_ADDR)

/* wakeup flag */
#define TLSR9118_PM_RRAM_WAKEUP_FLAG         (0)
#define TLSR9118_PM_RRAM_WAKEUP_WATCHER      (1 << 0)
#define TLSR9118_PM_RRAM_WAKEUP_RESET        (1 << 1)
#define TLSR9118_PM_RRAM_EXT_HIB_TIME		(1 << 2)

/* sync_flag */
#define TLSR9118_PM_RRAM_SYNC_FLAG           (1)
#define TLSR9118_PM_RRAM_SAVE_FLASH          (1 << 0)
#define TLSR9118_PM_RRAM_SAVE_CANCEL         (1 << 1)

/* watcher flag */
#define TLSR9118_PM_RRAM_WATCHER_FLAG        (2)
#define TLSR9118_PM_RRAM_WATCHER_EXEC        (1 << 0)
#define TLSR9118_PM_RRAM_WATCHER_SLEPT       (1 << 1)

/* pm state */
#define TLSR9118_PM_RRAM_MODE                (3)
#define TLSR9118_PM_RRAM_MODE_ACTIVE         (0)
#define TLSR9118_PM_RRAM_MODE_ILDE           (1)
#define TLSR9118_PM_RRAM_MODE_LIGHT_SLEEP    (2)
#define TLSR9118_PM_RRAM_MODE_SLEEP          (3)
#define TLSR9118_PM_RRAM_MODE_DEEP_SLEEP_0   (4)
#define TLSR9118_PM_RRAM_MODE_DEEP_SLEEP_1   (5)
#define TLSR9118_PM_RRAM_MODE_HIBERNATION    (6)

/* flash parameter */
#define TLSR9118_PM_RRAM_FLASH_RD_CMD        (5)
#define TLSR9118_PM_RRAM_FLASH_ER1_CMD       (6)
#define TLSR9118_PM_RRAM_FLASH_ER2_CMD       (7)
#define TLSR9118_PM_RRAM_FLASH_ER1_SIZE      (8)
#define TLSR9118_PM_RRAM_FLASH_ER2_SIZE      (12)
#define TLSR9118_PM_RRAM_FLASH_TIMING        (16)
#define TLSR9118_PM_RRAM_FLASH_CLK           (20)

#define TLSR9118_PM_RRAM_CRYPTO_CFG          (48)

#define TLSR9118_PM_RRAM_CRYPTO_KEY          (52)
#define TLSR9118_PM_RRAM_CRYPTO_KEY0         (52)
#define TLSR9118_PM_RRAM_CRYPTO_KEY1         (56)
#define TLSR9118_PM_RRAM_CRYPTO_KEY2         (60)
#define TLSR9118_PM_RRAM_CRYPTO_KEY3         (64)

#define TLSR9118_PM_RRAM_CRYPTO_IV           (68)
#define TLSR9118_PM_RRAM_CRYPTO_IV0         	(68)
#define TLSR9118_PM_RRAM_CRYPTO_IV1         	(72)
#define TLSR9118_PM_RRAM_CRYPTO_IV2         	(76)
#define TLSR9118_PM_RRAM_CRYPTO_IV3         	(80)

#ifndef __ASSEMBLY__

enum pm_feature {
	PM_FEATURE_VOLTAGE_CTRL 	= (1 << 0),
	PM_FEATURE_AON_VOLTAGE_CTRL = (1 << 1),
	PM_FEATURE_PLL_CTRL			= (1 << 2),
	PM_FEATURE_WAKEUP_IO_OFF	= (1 << 3),
	PM_FEATURE_FLASH_DPD		= (1 << 4),
	PM_FEATURE_WC_LOG			= (1 << 5),
	PM_FEATURE_RTC_FIX_RATIO	= (1 << 6),
	PM_FEATURE_WC_UART1			= (1 << 7),
	PM_FEATURE_DLDO_DCDC_CTRL	= (1 << 8),
	PM_FEATURE_AON_VOLTAGE_CTRL_OCR = (1 << 9),
	PM_FEATURE_WAKEUP_LOG_ON	= (1 << 10),
};

#define KEY_NUM 4

struct pm_rram_info
{
	uint8_t  wakeup_flag;                  /* 0 */
	uint8_t  sync_flag;                    /* 1 */
	uint8_t  exec_watcher;                 /* 2 */
	uint8_t  pm_mode;                      /* 3 */

	uint8_t  rsv1;                         /* 4 */
	/* TLSR9118_PM_RRAM_FLASH_RD_CMD         (5) */
	uint8_t  flash_mem_rd_cmd;
	/* TLSR9118_PM_RRAM_FLASH_ER1_CMD        (6) */
	uint8_t  flash_er1_cmd;
	/* TLSR9118_PM_RRAM_FLASH_ER2_CMD        (7) */
	uint8_t  flash_er2_cmd;

	/* TLSR9118_PM_RRAM_FLASH_ER1_SIZE       (8) */
	uint32_t flash_er1_size;
	/* TLSR9118_PM_RRAM_FLASH_ER2_SIZE      (12) */
	uint32_t flash_er2_size;
	/* TLSR9118_PM_RRAM_FLASH_TIMING        (16) */
	uint32_t flash_timing;
	/* TLSR9118_PM_RRAM_FLASH_CLK           (20) */
	uint32_t flash_clk;

	uint32_t rsv2;                        /* 24 */
	uint32_t rtc_value;                   /* 28 */
	uint64_t mtime_value;                 /* 32 */
	uint64_t next_mtime_value;            /* 40 */

	/* TLSR9118_PM_RRAM_CRYPTO_CFG          (48) */
	uint32_t crypto_cfg;
	/* TLSR9118_PM_RRAM_CRYPTO_KEY          (52) */
	/* TLSR9118_PM_RRAM_CRYPTO_KEY0         (52) */
	/* TLSR9118_PM_RRAM_CRYPTO_KEY1         (56) */
	/* TLSR9118_PM_RRAM_CRYPTO_KEY2         (60) */
	/* TLSR9118_PM_RRAM_CRYPTO_KEY3         (64) */
	uint32_t crypto_key[KEY_NUM];
	/* TLSR9118_PM_RRAM_CRYPTO_IV           (68) */
	/* TLSR9118_PM_RRAM_CRYPTO_IV0          (68) */
	/* TLSR9118_PM_RRAM_CRYPTO_IV0          (72) */
	/* TLSR9118_PM_RRAM_CRYPTO_IV0          (76) */
	/* TLSR9118_PM_RRAM_CRYPTO_IV0          (80) */
	uint32_t crypto_iv[KEY_NUM];

	uint32_t feature;                     /* 84 */

	uint32_t wk_entry;                    /* 88 */
	uint32_t wk_src;                      /* 92 */
	uint32_t wk_evt;                      /* 96 */
	uint16_t wk_fb_reason;               /* 100 */
	uint16_t wk_fb_type;                 /* 102 */
	uint64_t wk_time;                    /* 104 */
	uint32_t act_ratio;                  /* 112 */
	uint32_t ratio;                      /* 116 */
	uint64_t ratio_last_measure_time;    /* 120 */
	                                 /* end 128 */
}__attribute__((packed));

#define IS_FIXED_RATIO(r) (r->feature & PM_FEATURE_RTC_FIX_RATIO)

#endif /* __ASSEMBLY__ */

#endif /* _PM_RRAM_H_ */
