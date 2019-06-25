COMPILER = gcc -Wall

shell: main.o server.o error.o client.o bin
	$(COMPILER) -o bin/necho main.o server.o error.o client.o
	rm *.o

main.o: main.c error.h
	$(COMPILER) -o main.o -c main.c

server.o: server.c common.h
	$(COMPILER) -o server.o -c server.c

error.o: error.c error.h
	$(COMPILER) -o error.o -c error.c

client.o: client.c common.h
	$(COMPILER) -o client.o -c client.c
bin:	
	mkdir bin

clean:
	rm -f bin/necho
