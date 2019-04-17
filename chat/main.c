#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void start_server();
void send_message(const char* text);
void reg_on_server();
void chating();


int main(int argc, char** argv)
{
    if (argc > 1) {
		if (! strcmp(argv[1], "server")) {
			puts("Starting server");
			start_server();
		} else {
			//puts("Sending message");
			//send_message(argv[1]);
			reg_on_server(argv[1]);
			chating();
		}
	} else
		puts("Usage: \"chat server\" for server mode or \"chat _name_\" for client");

    return 0;
}
