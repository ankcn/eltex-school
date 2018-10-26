COMPILER = gcc -Wall

complex: menu.o libcalc.a main.o
	$(COMPILER) -o complex main.o -L. -lcalc

menu.o: menu.c menu.h complex.h
	$(COMPILER) -o menu.o -c menu.c

complex.o: complex.c complex.h
	$(COMPILER) -o complex.o -c complex.c

main.o: main.c complex.h menu.h
	$(COMPILER) -o main.o -c main.c

libcalc.a: complex.o menu.o
	ar rcs libcalc.a complex.o menu.o

clean:
	rm *.o *.a complex
