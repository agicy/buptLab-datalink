CC=gcc
CXX=g++
CFLAGS=-static -O2 -DNDEBUG
CXXFLAGS=-static -O2 -DNDEBUG

datalink: clean datalink.o protocol.o lprintf.o crc32.o crc_ec.o
	$(CXX) datalink.o protocol.o lprintf.o crc32.o crc_ec.o -o datalink -lm

datalink.o: datalink.c
	$(CC) $(CFLAGS) -DSEQ_BITS=$(SEQ_BITS) -DDATA_TIMER=$(DATA_TIMER) -DACK_TIMER=$(ACK_TIMER) -DCOMPACT_FRAME=$(COMPACT_FRAME) -c datalink.c

crc_ec.o: crc_ec.cpp
	$(CXX) $(CXXFLAGS) -DECC=$(ECC) -c crc_ec.cpp

clean:
	${RM} *.o datalink *.log

SEQ_BITS=
ifeq ($(SEQ_BITS),) 
	$(error SEQ_BITS is not set)
endif
ifeq ($(filter $(SEQ_BITS),1 2 3 4 5 6),)
	$(error SEQ_BITS must be between 1 and 6)
endif

DATA_TIMER=
ifeq ($(DATA_TIMER),)
	$(error DATA_TIMER is not set)
endif

ACK_TIMER=
ifeq ($(ACK_TIMER),)
	$(error ACK_TIMER is not set)
endif

COMPACT_FRAME=
ifeq ($(COMPACT_FRAME),)
	$(error COMPACT_FRAME is not set)
endif

ECC=
ifeq ($(ECC),)
	$(error ECC is not set)
endif
