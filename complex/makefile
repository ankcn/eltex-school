COMPILER = gcc -Wall
MOD_DIR = plugins

complex: menu.o main.o modules.o libcompl_mult.so libcompl_div.so libcompl_minus.so libcompl_plus.so
	$(COMPILER) -o complex main.o menu.o modules.o -ldl

menu.o: menu.c menu.h complex.h modules.h
	$(COMPILER) -o menu.o -c menu.c

main.o: main.c complex.h menu.h modules.h
	$(COMPILER) -o main.o -c main.c

modules.o: modules.c modules.h
	$(COMPILER) -o modules.o -c modules.c

libcompl_mult.so: compl_mult.o
	gcc -shared -o $(MOD_DIR)/libcompl_mult.so compl_mult.o

compl_mult.o: compl_mult.c complex.h
	$(COMPILER) -o compl_mult.o -c compl_mult.c

libcompl_div.so: compl_div.o
	gcc -shared -o $(MOD_DIR)/libcompl_div.so compl_div.o

compl_div.o: compl_div.c complex.h
	$(COMPILER) -o compl_div.o -c compl_div.c

libcompl_plus.so: compl_plus.o
	gcc -shared -o $(MOD_DIR)/libcompl_plus.so compl_plus.o

compl_plus.o: compl_plus.c complex.h
	$(COMPILER) -o compl_plus.o -c compl_plus.c
		
libcompl_minus.so: compl_minus.o
	gcc -shared -o $(MOD_DIR)/libcompl_minus.so compl_minus.o

compl_minus.o: compl_minus.c complex.h
	$(COMPILER) -o compl_minus.o -c compl_minus.c

clean:
	rm *.o complex $(MOD_DIR)/*
