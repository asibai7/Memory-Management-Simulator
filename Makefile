CC = gcc
CFLAGS = -Wall -Werror

memsim: memsim.o algo.o
	$(CC) $(CFLAGS) -o memsim memsim.o algo.o

memsim.o: memsim.c algo.c algo.h
	$(CC) $(CFLAGS) -c memsim.c

algo.o: algo.c algo.h
	$(CC) $(CFLAGS) -c algo.c

clean:
	rm -f memsim memsim.o algo.o
