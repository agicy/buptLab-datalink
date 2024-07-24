#ifndef CRC_EC_HEADER

#define CRC_EC_HEADER

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int crc_ec(uint8_t *message, const size_t len, const uint32_t crc);

#ifdef __cplusplus
}
#endif

#endif