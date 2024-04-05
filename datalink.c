#include "datalink.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>

#define DATA_TIMER 2000

struct FRAME {
    uint8_t kind; /* FRAME_DATA */
    uint8_t ack;
    uint8_t seq;
    uint8_t data[PKT_LEN];
    uint32_t padding;
};

static uint8_t frame_nr = 0, buffer[PKT_LEN], nbuffered;
static uint8_t frame_expected = 0;
static int phl_ready = 0;

static void put_frame(uint8_t *frame, int len) {
    *(uint32_t *)(frame + len) = crc32(frame, len);
    send_frame(frame, len + 4);
    phl_ready = 0;
}

static void send_data_frame(void) {
    struct FRAME s;

    s.kind = FRAME_DATA;
    s.seq = frame_nr;
    s.ack = 1 - frame_expected;
    memcpy(s.data, buffer, PKT_LEN);

    dbg_frame("Send DATA %d %d, ID %d\n", s.seq, s.ack, *(short *)s.data);

    put_frame((uint8_t *)&s, 3 + PKT_LEN);
    start_timer(frame_nr, DATA_TIMER);
}

static void send_ack_frame(void) {
    struct FRAME s;

    s.kind = FRAME_ACK;
    s.ack = 1 - frame_expected;

    dbg_frame("Send ACK  %d\n", s.ack);

    put_frame((uint8_t *)&s, 2);
}

int main(int argc, char **argv) {
    int event, arg;
    struct FRAME f;
    int len = 0;

    protocol_init(argc, argv);
    lprintf("Designed by Jiang Yanjun, build: " __DATE__ "  "__TIME__
            "\n");

    disable_network_layer();

    for (;;) {
        event = wait_for_event(&arg);

        switch (event) {
        case NETWORK_LAYER_READY:
            get_packet(buffer);
            nbuffered++;
            send_data_frame();
            break;

        case PHYSICAL_LAYER_READY:
            phl_ready = 1;
            break;

        case FRAME_RECEIVED:
            len = recv_frame((uint8_t *)&f, sizeof f);
            if (len < 5 || crc32((uint8_t *)&f, len) != 0) {
                dbg_event("**** Receiver Error, Bad CRC Checksum\n");
                break;
            }
            if (f.kind == FRAME_ACK)
                dbg_frame("Recv ACK  %d\n", f.ack);
            if (f.kind == FRAME_DATA) {
                dbg_frame("Recv DATA %d %d, ID %d\n", f.seq, f.ack, *(short *)f.data);
                if (f.seq == frame_expected) {
                    put_packet(f.data, len - 7);
                    frame_expected = 1 - frame_expected;
                }
                send_ack_frame();
            }
            if (f.ack == frame_nr) {
                stop_timer(frame_nr);
                nbuffered--;
                frame_nr = 1 - frame_nr;
            }
            break;

        case DATA_TIMEOUT:
            dbg_event("---- DATA %d timeout\n", arg);
            send_data_frame();
            break;
        }

        if (nbuffered < 1 && phl_ready)
            enable_network_layer();
        else
            disable_network_layer();
    }
}
