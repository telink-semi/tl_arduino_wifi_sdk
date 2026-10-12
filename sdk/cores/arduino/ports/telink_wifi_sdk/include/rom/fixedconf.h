/*******************************************************************************************************
 *
 * @file    fixedconf.h
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
 
#undef 	CONFIG_IFCONFIG_WME_FIX_AC
#define CONFIG_IFCONFIG_WME_FIX_AC 1

#undef 	CONFIG_IEEE80211W
#define CONFIG_IEEE80211W 1

#undef 	CONFIG_BEACON_PROTECT
#define CONFIG_BEACON_PROTECT 1

#undef 	CONFIG_SUPPORT_DUAL_VIF
#define CONFIG_SUPPORT_DUAL_VIF 1

#undef 	CONFIG_MAC_TX_TIMEOUT
#define CONFIG_MAC_TX_TIMEOUT 1

#undef 	CONFIG_SUPPORT_MU_EDCA
#define CONFIG_SUPPORT_MU_EDCA 1

#undef 	CONFIG_SUPPORT_MCC
#define CONFIG_SUPPORT_MCC 1

#undef 	CONFIG_SUPPORT_TWT
#define CONFIG_SUPPORT_TWT 1

#undef 	CONFIG_TLSR91X8_STATS
#define CONFIG_TLSR91X8_STATS 1

#undef 	CONFIG_TLSR91X8_CLI_MAC_DM
#define CONFIG_TLSR91X8_CLI_MAC_DM 1

#undef CONFIG_SUPPORT_DUAL_INSTANCE

#undef CONFIG_IEEE80211_FAST_TX
#define CONFIG_IEEE80211_FAST_TX 1

#undef CONFIG_SUPPORT_AMSDU_TX
#define CONFIG_SUPPORT_AMSDU_TX 1

#undef CONFIG_ACK_ENABLED_MULTI_TID
#define CONFIG_ACK_ENABLED_MULTI_TID 1

#undef CONFIG_IEEE80211_SOFT_WEP
#define CONFIG_IEEE80211_SOFT_WEP 1

#undef CONFIG_NET80211_PS_POLL
#define CONFIG_NET80211_PS_POLL 1
