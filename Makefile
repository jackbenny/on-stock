CC=cc
CFLAGS=-Wall -pedantic

on-stock: on-stock.o
	${CC} ${CFLAGS} on-stock.o -o on-stock

clean:
	rm -f on-stock.o on-stock
