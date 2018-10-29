COMPILER = gcc -Wall

complex: menu.o libcalc.so main.o
	$(COMPILER) -o complex main.o -L. -lcalc -Wl,-rpath,.

menu.o: menu.c menu.h complex.h
	$(COMPILER) -fPIC -o menu.o -c menu.c

complex.o: complex.c complex.h
	$(COMPILER) -fPIC -o complex.o -c complex.c

main.o: main.c complex.h menu.h
	$(COMPILER) -o main.o -c main.c

libcalc.so: complex.o menu.o
	gcc -shared -o libcalc.so complex.o menu.o

clean:
	rm *.o *.a *.so complex
