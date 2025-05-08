CC := $(CROSS_COMPILE)gcc
CFLAGS := -Wall -Werror -g

TARGET = aesdsocket
SRC = aesdsocket.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	
distclean: clean
	
.PHONY: all default clean distclean
