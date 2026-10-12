#ifndef _WITS_CRC16_H__
#define __WITS_CRC16_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t crc16_ccitt(void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __WITS_CRC16_H__ */
