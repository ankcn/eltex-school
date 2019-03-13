COMPILER = gcc -Wall

shell: main.o stroka.o history.o interface.o bin
	$(COMPILER) -o bin/shell main.o stroka.o history.o interface.o -pthread
	rm *.o

main.o: main.c interface.h
	$(COMPILER) -o main.o -c main.c

stroka.o: stroka.c stroka.h
	$(COMPILER) -o stroka.o -c stroka.c

history.o: history.c history.h
	$(COMPILER) -o history.o -c history.c

interface.o: interface.c interface.h stroka.h history.h
	$(COMPILER) -o interface.o -c interface.c

bin:	
	mkdir bin

clean:
	rm -f bin/shell
