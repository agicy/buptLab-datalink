#pragma once

#ifndef DATALINK_H
#define DATALINK_H

#ifndef SEQ_BITS
#define SEQ_BITS 6
#endif

#ifndef DATA_TIMER
#define DATA_TIMER 1390
#endif

#ifndef ACK_TIMER
#define ACK_TIMER 278
#endif

/* FRAME kind */
#define FRAME_SACK 0
#define FRAME_DATA 1
#define FRAME_ACK 2
#define FRAME_NAK 3

/*
    SACK Frame
    +=========+========+========+
    | KIND(1) | ACK(1) | CRC(4) |
    +=========+========+========+

    DATA Frame
    +=========+========+========+===============+========+
    | KIND(1) | ACK(1) | SEQ(1) | DATA(240~256) | CRC(4) |
    +=========+========+========+===============+========+

    ACK Frame
    +=========+========+========+
    | KIND(1) | ACK(1) | CRC(4) |
    +=========+========+========+

    NAK Frame
    +=========+========+========+
    | KIND(1) | ACK(1) | CRC(4) |
    +=========+========+========+
*/

#endif
