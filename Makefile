CC=gcc
CXX=g++
CFLAGS=-static -O2 -DNDEBUG 

datalink: datalink.o protocol.o lprintf.o crc32.o crc_ec.o
	$(CXX) datalink.o protocol.o lprintf.o crc32.o crc_ec.o -o datalink -lm

crc_ec.o: crc_ec.cpp
	$(CXX) $(CFLAGS) -c crc_ec.cpp

clean:
	${RM} *.o datalink *.log
