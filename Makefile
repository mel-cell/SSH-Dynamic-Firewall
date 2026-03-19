CC = gcc
CFLAGS = -Wall -Wextra -Isrc
LDFLAGS = 

SRC = src/main.o src/netutil.o src/logger.o src/lock.o src/ipstore.o src/firewall.o
MONITOR_SRC = src/monitor.o

all: ssh-firewall ssh-monitor

ssh-firewall: $(SRC)
	$(CC) $(CFLAGS) -o ssh-firewall $(SRC) $(LDFLAGS)

ssh-monitor: $(MONITOR_SRC)
	$(CC) $(CFLAGS) -o ssh-monitor $(MONITOR_SRC) $(LDFLAGS)

clean:
	rm -f src/*.o ssh-firewall ssh-monitor
