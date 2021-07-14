#ifndef NET_UTILITIES_H_
#define NET_UTILITIES_H_
//--------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

//--------------------------------------------------
uint16_t checksum(uint8_t *ptr, uint16_t len, uint8_t type);
uint32_t crc32 (const unsigned char *buf, int len, unsigned int init);
//void rmemcpy32(uint8_t *restrict dst, const uint32_t *restrict src, uint8_t size);
void rmemcpy(uint8_t *dst, const uint8_t *src, uint8_t size);
//--------------------------------------------------

#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define be32todword(a) ((((a)>>24)&0xff)|(((a)>>8)&0xff00)|(((a)<<8)&0xff0000)|(((a)<<24)&0xff000000))

//--------------------------------------------------

#endif
