CC := gcc
CFLAGS := -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -O2 -g
SRC := main.c
OBJ := $(SRC:.c=.o)

all: scheduler

scheduler: $(OBJ)
	$(CC) $(CFLAGS) -o scheduler $(OBJ)

%.o: %.c scheduler.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f scheduler $(OBJ) rate_afsr.out edf_afsr.out
