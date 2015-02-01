CC = gcc

CFLAGS  =

TARGET = findrep

DEPS = grep-rl.c sed.c

HDEPS = grep-rl.h sed.h

all: $(TARGET)

$(TARGET): $(TARGET).c $(DEPS) $(HDEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(DEPS)

