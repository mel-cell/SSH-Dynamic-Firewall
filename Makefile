CC = gcc
CFLAGS = -Wall -Wextra -Isrc
SRC = src/main.c src/netutil.c src/logger.c src/lock.c src/ipstore.c src/firewall.c
OBJ = $(SRC:.c=.o)
TARGET = ssh-firewall

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
