/*******************************************************************************************************
 *
 * @file    lwip-glue.h
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
 
#ifndef _FBSD_LWIP_GLUE_H_
#define _FBSD_LWIP_GLUE_H_

#include <sys/queue.h>
#include "compat_types.h"
#include "systm.h"
#include "compat_param.h"
#include "mbuf.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"

extern struct mbuf*	(*m_frompbuf)(struct pbuf *, int head);

extern struct pbuf*	(*m_topbuf)(struct mbuf *);


#endif	/* _FBSD_LWIP_GLUE_H_ */
