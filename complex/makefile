COMPILER = gcc -Wall

complex: menu.o complex.o main.o
	$(COMPILER) -o complex menu.o complex.o main.o

menu.o: menu.c menu.h complex.h
	$(COMPILER) -o menu.o -c menu.c

complex.o: complex.c complex.h
	$(COMPILER) -o complex.o -c complex.c

main.o: main.c complex.h menu.h
	$(COMPILER) -o main.o -c main.c

clean:
	rm *.o complex
