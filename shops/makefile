COMPILER = gcc -Wall

shops: main.o shop.o buyer.o loader.o utils.o bin
	$(COMPILER) -o bin/shops main.o shop.o buyer.o loader.o utils.o -pthread
	rm *.o

main.o: main.c shop.h buyer.h loader.h utils.h
	$(COMPILER) -o main.o -c main.c

shop.o: shop.c shop.h utils.h
	$(COMPILER) -o shop.o -c shop.c

buyer.o: buyer.c buyer.h shop.h utils.h
	$(COMPILER) -o buyer.o -c buyer.c

loader.o: loader.c loader.h shop.h utils.h
	$(COMPILER) -o loader.o -c loader.c

bin:	
	mkdir bin

clean:
	rm -f bin/shops
