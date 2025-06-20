CC=gcc
CFLAGS=-O3 -Wall -Wextra -Wpedantic
LDFLAGS=-lwinmm
TARGET=sim_waveserver.exe
SRCS=main.c framer.c overhead.c performance.c

all: $(TARGET)

$(TARGET): $(SRCS) otn.h
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)

clean:
	rm $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)
