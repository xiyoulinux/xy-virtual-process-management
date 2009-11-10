all: virtualprocess

CC= gcc
CFLAGS= -Wall -g 
INCLUDE= ./include

objs= main.o kernel.a
virtualprocess: $(objs)
	$(CC) $(CFLAGS) -I$(INCLUDE) $@ @?
main.o: main.c 
	$(CC) $(CFLAGS) -I$(INCLUDE) $@ @<
kernel.a:
	(cd kernel;make;mv kernel.a ..)

.PHONY: clean distclean
clean:
	@rm *.o *.a
distclean: clean
	@rm virtualprocess
