#pragma once

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define VERSION "4.1"

#include "lprintf.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initalization */
extern void protocol_init(int argc, char **argv);

/* Event Driver */
extern int wait_for_event(int *arg);

#define NETWORK_LAYER_READY 0
#define PHYSICAL_LAYER_READY 1
#define FRAME_RECEIVED 2
#define DATA_TIMEOUT 3
#define ACK_TIMEOUT 4

/* Network Layer functions */
#define PKT_LEN 256

extern void enable_network_layer(void);
extern void disable_network_layer(void);
extern int get_packet(uint8_t *packet);
extern void put_packet(uint8_t *packet, int len);

/* Physical Layer functions */
extern int recv_frame(uint8_t *buf, int size);
extern void send_frame(uint8_t *frame, int len);

extern int phl_sq_len(void);

/* CRC-32 polynomium coding function */
extern uint32_t crc32(const uint8_t *buf, size_t len);

/* Timer Management functions */
extern uint32_t get_ms(void);
extern void start_timer(uint32_t nr, uint32_t ms);
extern void stop_timer(uint32_t nr);
extern void start_ack_timer(uint32_t ms);
extern void stop_ack_timer(void);

/* Protocol Debugger */
extern const char *station_name(void);

extern void dbg_event(const char *fmt, ...);
extern void dbg_frame(const char *fmt, ...);
extern void dbg_warning(const char *fmt, ...);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
