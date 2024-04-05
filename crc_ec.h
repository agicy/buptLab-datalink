#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int crc_ec(uint8_t *message, size_t len, uint32_t crc);

#ifdef __cplusplus
}
#endif
