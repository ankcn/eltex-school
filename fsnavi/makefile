COMPILER = gcc -Wall

fsnavi: manager.o main.o bin
	$(COMPILER) -o bin/fsnavi manager.o main.o -lncurses -pthread
	rm *.o

manager.o: manager.c manager.h
	$(COMPILER) -o manager.o -c manager.c

main.o: main.c manager.h
	$(COMPILER) -o main.o -c main.c

bin:	
	mkdir bin

clean:
	rm bin/fsnavi
