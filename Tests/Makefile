# $Id: Makefile,v 1.1 2001/09/29 01:58:59 jzweizig Exp $
CC = gcc
#CFLAGS = -DDEBUG -g -I. -I/usr/local/include -I$(DMTHOME)/include -L. -L/usr/local/lib -L$(DMTHOME)/lib
CFLAGS = -O3 -I. -I/usr/local/include -I$(DMTHOME)/include -L. -L/usr/local/lib -L$(DMTHOME)/lib -L/export/home/dmt/lib

RM = /bin/rm -f

.c.o:
	$(CC) -c $(CFLAGS) $?

all: makeTestData readback

makeTestData: makeTestData.o frameint.o
	$(CC) $(CFLAGS) -o makeTestData makeTestData.o frameint.o -lFrame -lm

readback: readback.o frameint.o
	$(CC) $(CFLAGS) -o readback readback.o frameint.o -lFrame -lm

clean:
	$(RM) *.o makeTestData readback *~ output *.lis *.out

realclean: clean
	$(RM) -r Datadir
