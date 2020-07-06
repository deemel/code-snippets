#
#    Makefile for BigInt
#

CC=CC                        # change if your C++ compiler is different
FLAGS=                       # compiler flags, i.e. -g -DDEBUG

a.out: bigint.o bigint.t.o cex_memory.o cex_timer.o
	$(CC) $(FLAGS) bigint.o bigint.t.o cex_memory.o cex_timer.o -o a.out

bigint.o: bigint.c bigint.h
	$(CC) $(FLAGS) bigint.c -c

bigint.t.o: bigint.t.c bigint.h
	$(CC) $(FLAGS)  bigint.t.c -c

cex_timer.o: cex_timer.c cex_timer.h
	$(CC) $(FLAGS)  cex_timer.c -c

cex_memory.o: cex_memory.c cex_memory.h
	$(CC) $(FLAGS)  cex_memory.c -c

submit:
	/b/u2/library/EN/cs6999/bin/submit

