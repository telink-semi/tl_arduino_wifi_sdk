/*******************************************************************************************************
 *
 * @file    head.h
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
 #ifndef __HEAD_H__
#define __HEAD_H__

/* TLSR9118 flash boot record */
typedef struct {
	unsigned magic;
	unsigned hcrc32;
	unsigned ver;

	unsigned size;
	unsigned lma;
	unsigned vma;
	unsigned dcrc32;

	unsigned char os;
	unsigned char arch;
	unsigned char type; /* 0 : bootable, 1 : non-bootable */
	unsigned char comp;

	/* secure boot */
	unsigned char signature[64]; /* [32:95] */
	unsigned char pk[64]; /* [96:159] */
} boot_hdr_t;

typedef union {
	boot_hdr_t bhdr;
	char buffer[160];
} img_hdr_t;

#define uswap_32(x) \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | \
	 (((x) & 0x000000ff) << 24))

#endif /* __HEAD_H__ */
