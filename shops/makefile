COMPILER = gcc -Wall

shops: main.o bin
	$(COMPILER) -o bin/shops main.o -pthread
	rm *.o

main.o: main.c 
	$(COMPILER) -o main.o -c main.c

bin:	
	mkdir bin

clean:
	rm -f bin/shops
