COMPILER = gcc -Wall

fsnavi: manager.o main.o
	$(COMPILER) -o fsnavi manager.o main.o -lncurses -pthread

manager.o: manager.c manager.h
	$(COMPILER) -o manager.o -c manager.c

main.o: main.c manager.h
	$(COMPILER) -o main.o -c main.c

clean:
	rm *.o fsnavi
