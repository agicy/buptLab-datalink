#pragma once

#ifndef CRC_EC_H
#define CRC_EC_H

#ifndef ECC
#define ECC 1
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int crc_ec(uint8_t *message, const size_t len, const uint32_t crc);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
