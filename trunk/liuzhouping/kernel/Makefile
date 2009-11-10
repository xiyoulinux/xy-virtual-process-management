
CC= gcc
CFLAGS= -Wall -g 
INCLUDE= ../include
AR= ar
ARFLAGS= crv

objs= myfork.o 

kernel.a: $(objs)
	$(AR) $(ARFLAGS) 
main.o: main.c 
	$(CC) $(CFLAGS) -I$(INCLUDE) $@ @<
kernel.a:
	(cd kernel;make;mv kernel.a ..)

.PHONY: clean 
clean:
	@rm *.o *.a
