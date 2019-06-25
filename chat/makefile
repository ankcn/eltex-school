COMPILER = gcc -Wall

shell: main.o client.o timestamp.o noecho.o bin
	$(COMPILER) -o bin/chat main.o client.o timestamp.o noecho.o -lpthread -lrt
	rm *.o

main.o: main.c common.h
	$(COMPILER) -o main.o -c main.c

noecho.o: noecho.c noecho.h
	$(COMPILER) -o noecho.o -c noecho.c

timestamp.o: timestamp.c timestamp.h
	$(COMPILER) -o timestamp.o -c timestamp.c

bin:	
	mkdir bin

clean:
	rm -f bin/chat
