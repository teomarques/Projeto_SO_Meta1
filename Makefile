CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: controller miner validator statistics TxGen

controller: controller.c logging.c
	$(CC) $(CFLAGS) -o $@ $^

miner: miner.c logging.c
	$(CC) $(CFLAGS) -o $@ $^

validator: validator.c logging.c
	$(CC) $(CFLAGS) -o $@ $^

statistics: statistics.c logging.c
	$(CC) $(CFLAGS) -o $@ $^

TxGen: txgen.c logging.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f controller miner validator statistics TxGen *.o

.PHONY: all clean