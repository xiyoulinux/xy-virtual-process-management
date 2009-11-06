main : main.o fork.o
	gcc -o main main.o fork.o
main.o : main.c fork.c
	gcc -c main.c fork.c
.PHONY : clean
clean:
	-rm -f *.o
