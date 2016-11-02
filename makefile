project : mine1.o 
	cc mine1.o -o project -lncurses
project2.o: mine1.c
	cc mine1.c -Wall -lncurses -c 
clean : 
	rm *.o

