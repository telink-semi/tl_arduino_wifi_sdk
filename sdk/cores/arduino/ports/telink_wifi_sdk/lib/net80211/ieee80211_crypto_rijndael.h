/*******************************************************************************************************
 *
 * @file    ieee80211_crypto_rijndael.h
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
 
#ifndef _NET80211_IEEE80211_CRYPTO_RIJNDAEL_H_
#define _NET80211_IEEE80211_CRYPTO_RIJNDAEL_H_

#include <crypto/rijndael-alg-fst.h>

#define RIJNDAEL_MAXKC	(256/32)
#define RIJNDAEL_MAXKB	(256/8)
#define RIJNDAEL_MAXNR	14

typedef struct {
	int	decrypt;
	int	Nr;		/* key-length-dependent number of rounds */
	u32 ek[4 * (RIJNDAEL_MAXNR + 1)];	/* encrypt key schedule */
	u32 dk[4 * (RIJNDAEL_MAXNR + 1)];	/* decrypt key schedule */
} rijndael_ctx;

static __inline void
rijndael_set_key(rijndael_ctx *ctx, const u_char *key, int bits)
{
	ctx->Nr = rijndaelKeySetupEnc(ctx->ek, key, bits);
	rijndaelKeySetupDec(ctx->dk, key, bits);
}

static __inline void
rijndael_decrypt(const rijndael_ctx *ctx, const u_char *src, u_char *dst)
{

	rijndaelDecrypt(ctx->dk, ctx->Nr, src, dst);
}

static __inline void
rijndael_encrypt(const rijndael_ctx *ctx, const u_char *src, u_char *dst)
{
	rijndaelEncrypt(ctx->ek, ctx->Nr, src, dst);
}

#endif /* _NET80211_IEEE80211_CRYPTO_RIJNDAEL_H_ */
