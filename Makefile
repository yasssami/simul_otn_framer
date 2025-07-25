CC=gcc
CFLAGS=-O3 -Wall -Wextra -Wpedantic
LDFLAGS=-lwinmm
TARGET=sim_waveserver.exe
SRCS=main.c framer.c overhead.c performance.c

OBJS=$(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c otn.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) 2>/dev/null || true

run: $(TARGET)
	./$(TARGET)
