#
# Makefile for CS314 Project - user process
# Jacques Eloff (eloff@cs.sun.ac.za)
# University of Stellenbosch
# 10.02.2005

# C compiler
CC = gcc
LD = ld

# Flags for the C compiler
CFLAGS = -Wall -W -g -minline-all-stringops

# Linker flags
LDFLAGS = -static -nostdlib --no-strip-discarded
RM = rm -f

# Library files used to access kernel functions 
LIB_OBJS = crt.o ipc.o
 
# User process implementation files


NAME = $(PROCESS_1)

all: proc1 proc2 proc3 proc3 proc4 proc5 proc6 proc7 proc8 proc9

LIBS: $(LIB_OBJS)
	$(CC) $(CFLAGS)

proc%: $(LIB_OBJS) proc%.o
	./gen_script $@.ld $@.o $(LIB_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-T,$@.ld -Wl,-Map,$@.map -o $@

clean:
	$(RM) *.{o,map,s,bin,ld} *~
	$(RM) $(NAME)
