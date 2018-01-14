# EnderUNIX Aget Makefile
# http://www.enderunix.org/aget/

OBJS = main.o Aget.o Misc.o Head.o Signal.o Download.o Resume.o
CFLAGS = -g -W -D_GNU_SOURCE
LDFLAGS = -pthread
CC = gcc
STRIP = strip

all: $(OBJS)
	$(CC) -o aget $(OBJS) $(LDFLAGS)

strip: $(all)
	$(STRIP) aget
	
install:
	cp -f aget /usr/local/bin/aget

clean: 
	rm -f aget *.o core.* *~

c:
	rm -f *core* *~ *log
