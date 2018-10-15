COMPILER = gcc -Wall

phonebook: menu.o phonebook.o main.o
	$(COMPILER) -o phonebook menu.o phonebook.o main.o

menu.o: menu.c menu.h phonebook.h
	$(COMPILER) -o menu.o -c menu.c

phonebook.o: phonebook.c phonebook.h
	$(COMPILER) -o phonebook.o -c phonebook.c

main.o: main.c phonebook.h menu.h
	$(COMPILER) -o main.o -c main.c

clean:
	rm *.o phonebook
