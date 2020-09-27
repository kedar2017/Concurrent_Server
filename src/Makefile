CC = gcc
CFLAGS = -Wall
OBJS = wserver.o wclient.o request.o io_helper.o thread.o

.SUFFIXES: .c .o

all: wserver wclient spin.cgi

wserver: wserver.o request.o io_helper.o thread.o
	$(CC) $(CFLAGS) -o wserver wserver.o request.o io_helper.o thread.o

wclient: wclient.o io_helper.o
	$(CC) $(CFLAGS) -o wclient wclient.o io_helper.o

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) wserver wclient spin.cgi
