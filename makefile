project : mine.o 
	cc mine.o -o project -lncurses
project2.o: mine.c
	cc mine.c -Wall -lncurses -c 
clean : 
	rm *.o

