#include "datalink.h"
#include "crc_ec.hpp"
#include "protocol.h"
#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void handler(int signum) {
    lprintf("signal %d raised!\n", signum);
    fflush(stdout);
    if (log_file)
        fflush(log_file);
    exit(0);
}

static int phl_ready = 0;

static void put_frame(uint8_t *fp, int len) {
    *(uint32_t *)(fp + len) = crc32(fp, len);
    send_frame(fp, len + 4);
    phl_ready = 0;
}

#define MAX_SEQ ((1 << SEQ_BITS) - 1)
#define NR_BUFS ((MAX_SEQ + 1) / 2)

typedef uint8_t seq_nr;
typedef uint8_t packet_t[PKT_LEN];

#if COMPACT_FRAME

#define KIND_ACK_SIZE 1

typedef struct frame {
    uint8_t kind : 2;
    uint8_t ack : 6;
    seq_nr seq;
    packet_t data;
    uint32_t padding;
} frame;

#else

#define KIND_ACK_SIZE 2

typedef struct frame {
    uint8_t kind;
    seq_nr ack;
    seq_nr seq;
    packet_t data;
    uint32_t padding;
} frame;

#endif

typedef struct window {
    seq_nr begin, end;
    uint8_t size;
    packet_t buffer[NR_BUFS];
} window;

static inline int between(const window *const wp, seq_nr id) {
    return (wp->begin <= id && id < wp->end) || (wp->begin > wp->end && wp->begin <= id) || (wp->begin > wp->end && id < wp->end);
}

static inline int full(const window *const wp) {
    return wp->size == NR_BUFS;
}

static inline seq_nr prev(const seq_nr id) {
    return id == 0 ? MAX_SEQ : id - 1;
}

static inline seq_nr next(const seq_nr id) {
    return id == MAX_SEQ ? 0 : id + 1;
}

int arrived[NR_BUFS];
int no_nak;
window sender, receiver;

static inline void handle_data_frame(seq_nr seq, seq_nr expected_id) {
    frame f;
    f.kind = FRAME_DATA;
    f.ack = expected_id;
    f.seq = seq;
    memcpy(f.data, sender.buffer[seq % NR_BUFS], sizeof(f.data));
    put_frame((uint8_t *)&f, KIND_ACK_SIZE + sizeof(f.seq) + sizeof(f.data));
    return;
}

static inline void send_data_frame(seq_nr seq, seq_nr expected_id) {
    handle_data_frame(seq, expected_id);
    start_timer(seq, DATA_TIMER);
    stop_ack_timer();
}

static inline void handle_ack_frame(seq_nr expected_id) {
    frame f;
    f.kind = FRAME_ACK;
    f.ack = expected_id;
    put_frame((uint8_t *)&f, KIND_ACK_SIZE);
}

static inline void send_ack_frame(seq_nr expected_id) {
    handle_ack_frame(expected_id);
}

static inline void handle_nak_frame(seq_nr expected_id) {
    frame f;
    f.kind = FRAME_NAK;
    f.ack = expected_id;
    put_frame((uint8_t *)&f, KIND_ACK_SIZE);
}

static inline void send_nak_frame(seq_nr expected_id) {
    no_nak = 0;
    handle_nak_frame(expected_id);
}

static inline void handle_sack_frame(seq_nr ack_id) {
    frame f;
    f.kind = FRAME_SACK;
    f.ack = ack_id;
    put_frame((uint8_t *)&f, KIND_ACK_SIZE);
}

static inline void send_sack_frame(seq_nr ack_id) {
    handle_sack_frame(ack_id);
}

int main(int argc, char **argv) {
    signal(SIGTERM, handler);
    protocol_init(argc, argv);

    int event, arg;
    frame f;
    int len;
    uint32_t checksum;

    lprintf("Designed by Lu Anlai, build: " __DATE__ "\n");

    enable_network_layer();
    receiver.end = NR_BUFS;
    receiver.size = NR_BUFS;
    for (;;) {
        event = wait_for_event(&arg);

        switch (event) {
        case NETWORK_LAYER_READY:
            dbg_event("---- NETWORK_LAYER_READY\n");
            get_packet(sender.buffer[sender.end % NR_BUFS]);
            send_data_frame(sender.end, receiver.begin);

            sender.end = next(sender.end);
            ++sender.size;
            break;

        case PHYSICAL_LAYER_READY:
            dbg_event("---- PHYSICAL_LAYER_READY\n");
            phl_ready = 1;
            break;

        case FRAME_RECEIVED:
            len = recv_frame((uint8_t *)&f, sizeof f);
            checksum = crc32((uint8_t *)&f, len);
            if (checksum)
                if (crc_ec((uint8_t *)&f, len, checksum)) {
                    dbg_event("**** Receiver Error, Bad CRC Checksum\n");
                    if (no_nak)
                        send_nak_frame(receiver.begin);
                    break;
                }

            switch (f.kind) {

            case FRAME_DATA:
                dbg_frame("Recv DATA %d %d, ID %d\n", f.seq, f.ack, *(short *)f.data);
                if (between(&receiver, f.seq)) {
                    if (f.seq != receiver.begin && no_nak)
                        send_nak_frame(receiver.begin);
                    if (!arrived[f.seq % NR_BUFS]) {
                        arrived[f.seq % NR_BUFS] = 1;
                        memcpy(receiver.buffer[f.seq % NR_BUFS], f.data, sizeof(f.data));
                        if (f.seq != receiver.begin)
                            send_sack_frame(f.seq);
                        else
                            while (arrived[receiver.begin % NR_BUFS]) {
                                put_packet(receiver.buffer[receiver.begin % NR_BUFS], PKT_LEN);
                                no_nak = 1;
                                arrived[receiver.begin % NR_BUFS] = 0;
                                receiver.begin = next(receiver.begin), receiver.end = next(receiver.end);
                                start_ack_timer(ACK_TIMER);
                            }
                    }
                }
                break;

            case FRAME_ACK:
                dbg_frame("Recv ACK %d\n", f.ack);
                break;

            case FRAME_NAK:
                dbg_frame("Recv NAK %d\n", f.ack);
                assert(between(&sender, f.ack));
                send_data_frame(f.ack, receiver.begin);
                break;

            case FRAME_SACK:
                dbg_frame("Recv SACK %d\n", f.ack);
                stop_timer(f.ack);
                break;
            }

            if (f.kind != FRAME_SACK)
                while (between(&sender, prev(f.ack))) {
                    stop_timer(sender.begin);
                    sender.begin = next(sender.begin);
                    --sender.size;
                }
            break;

        case DATA_TIMEOUT:
            dbg_event("---- DATA %d timeout\n", arg);
            send_data_frame(arg, receiver.begin);
            break;

        case ACK_TIMEOUT:
            dbg_event("---- ACK %d timeout\n", arg);
            send_ack_frame(receiver.begin);
            break;
        }

        if (full(&sender))
            dbg_event("---- sender window is already full!\n");

        if (!full(&sender) && phl_ready)
            enable_network_layer();
        else
            disable_network_layer();
    }

    return 0;
}
