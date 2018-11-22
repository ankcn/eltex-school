COMPILER = gcc -Wall

myedit: editor.o files.o main.o
	$(COMPILER) -o myedit editor.o files.o main.o -lncurses

editor.o: editor.c editor.h files.h
	$(COMPILER) -o editor.o -c editor.c

files.o: files.c files.h
	$(COMPILER) -o files.o -c files.c

main.o: main.c editor.h
	$(COMPILER) -o main.o -c main.c

clean:
	rm *.o myedit
