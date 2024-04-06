CC=gcc
CXX=g++
CFLAGS=-static -O2 -DNDEBUG
CXXFLAGS=-static -O2 -DNDEBUG

TEST_TIME=1800
SLEEP_TIME=2100

test: datalink
	screen -dmS datalinkA bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink a --utopia --log=u_a.log';
	screen -dmS datalinkB bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink b --utopia --log=u_b.log';
	sleep $(SLEEP_TIME);
	screen -dmS datalinkA bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink a --log=_a.log';
	screen -dmS datalinkB bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink b --log=_b.log';
	sleep $(SLEEP_TIME);
	screen -dmS datalinkA bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink a --flood --utopia --log=fu_a.log';
	screen -dmS datalinkB bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink b --flood --utopia --log=fu_b.log';
	sleep $(SLEEP_TIME);
	screen -dmS datalinkA bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink a --flood --log=f_a.log';
	screen -dmS datalinkB bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink b --flood --log=f_b.log';
	sleep $(SLEEP_TIME);
	screen -dmS datalinkA bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink a --flood --ber=1e-4 --log=fb1e-4_a.log';
	screen -dmS datalinkB bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink b --flood --ber=1e-4 --log=fb1e-4_b.log';

datalink: clean datalink.o protocol.o lprintf.o crc32.o crc_ec.o
	mkdir $(OUTPUT_DIR) -p
	$(CXX) datalink.o protocol.o lprintf.o crc32.o crc_ec.o -o $(OUTPUT_DIR)/datalink -lm

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

ifeq ($(COMPACT_FRAME),1)
ifeq ($(filter $(SEQ_BITS),1 2 3 4 5 6),)
$(error SEQ_BITS must be between 1 and 6 when using COMPACT_FRAME)
endif
else
ifeq ($(filter $(SEQ_BITS),1 2 3 4 5 6 7 8),)
$(error SEQ_BITS must be between 1 and 8)
endif
endif

ECC=
ifeq ($(ECC),)
$(error ECC is not set)
endif

OUTPUT_DIR=TEST_$(SEQ_BITS)BITS_$(DATA_TIMER)DATA_$(ACK_TIMER)ACK
ifeq ($(COMPACT_FRAME),1)
OUTPUT_DIR+=_COMPACT
endif
ifeq ($(ECC),1)
OUTPUT_DIR+=_ECC
endif

space=$(empty) $(empty)
OUTPUT_DIR:=$(subst $(space),,$(OUTPUT_DIR))
