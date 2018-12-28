COMPILER = gcc -Wall

myedit: editor.o files.o main.o bin
	$(COMPILER) -o bin/myedit bin/editor.o bin/files.o bin/main.o -lncurses

editor.o: editor.c editor.h files.h bin
	$(COMPILER) -o bin/editor.o -c editor.c

files.o: files.c files.h bin
	$(COMPILER) -o bin/files.o -c files.c

main.o: main.c editor.h bin
	$(COMPILER) -o bin/main.o -c main.c

clean:
	rm bin/*.o bin/myedit

bin:	
	mkdir bin
