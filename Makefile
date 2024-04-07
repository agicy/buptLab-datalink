# Set the C compiler
CC=gcc

# Set the flags for the C compiler
CFLAGS=-static -Ofast -Wall -Wextra -Werror

# Check if TEST_TIME is not set
TEST_TIME=
ifeq ($(TEST_TIME),)
TEST_TIME=600
endif

# Define the options for the datalink program
opt1=--port=10001 --utopia
opt2=--port=10002
opt3=--port=10003 --flood --utopia
opt4=--port=10004 --flood
opt5=--port=10005 --flood --ber=1e-4

# Define the test target
test: datalink
	# For each option, run the datalink program in a new screen session
	@$(foreach i,$(shell seq 1 5),\
		screen -dmS $(i)_datalinkA bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink a $(opt$(i)) --log=$(i)_a.log; exit';\
		screen -dmS $(i)_datalinkB bash -c 'cd $(OUTPUT_DIR); timeout $(TEST_TIME) ./datalink b $(opt$(i)) --log=$(i)_b.log; exit';\
	)
	# Sleep for TEST_TIME seconds
	sleep $(TEST_TIME)
	# Remove all object files
	${RM} *.o
	# Print the last line of each log file
	@$(foreach i,$(shell seq 1 5),\
		echo $$(tail -n 2 $(OUTPUT_DIR)/$(i)_a.log | head -n 1 | awk -F',' '{print $$3}' | awk '{print $$1}') $$(tail -n 2 $(OUTPUT_DIR)/$(i)_b.log | head -n 1 | awk -F',' '{print $$3}' | awk '{print $$1}');\
	)

# Define the datalink target
datalink: clean datalink.o protocol.o lprintf.o crc32.o
	# Create the output directory if it does not exist
	mkdir $(OUTPUT_DIR) -p
	# Link the object files to create the datalink executable
	$(CC) datalink.o protocol.o lprintf.o crc32.o -o $(OUTPUT_DIR)/datalink -lm

# Compile the datalink.c file into an object file
datalink.o: datalink.c
	$(CC) $(CFLAGS) -DSEQ_BITS=$(SEQ_BITS) -DDATA_TIMER=$(DATA_TIMER) -DACK_TIMER=$(ACK_TIMER) -c datalink.c

# Define the clean target to remove all object files
clean:
	${RM} *.o datalink *.log

# Check if SEQ_BITS is not set
SEQ_BITS=
ifeq ($(SEQ_BITS),)
SEQ_BITS=6
endif

# Check if DATA_TIMER is not set
DATA_TIMER=
ifeq ($(DATA_TIMER),)
DATA_TIMER=1390
endif

# Check if ACK_TIMER is not set
ACK_TIMER=
ifeq ($(ACK_TIMER),)
ACK_TIMER=278
endif

# Check if SEQ_BITS is between 1 and 6, if not, throw an error
ifeq ($(filter $(SEQ_BITS),1 2 3 4 5 6),)
$(error SEQ_BITS must be between 1 and 6)
endif

# Define the output directory name based on the parameters
OUTPUT_DIR=TEST_$(SEQ_BITS)BITS_$(DATA_TIMER)DATA_$(ACK_TIMER)ACK

# Remove spaces from the output directory name
space=$(empty) $(empty)
OUTPUT_DIR:=$(subst $(space),,$(OUTPUT_DIR))
