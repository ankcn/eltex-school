#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "shell.h"
#include "interface.h"


const char* CMD_CD = "cd";


void parse_command(char* cmd)
{
	char* varg[MAX_ARGS];
	char* part = strtok(cmd, " \t");
	unsigned i = 0;

	while (part != NULL) {
		varg[i] = malloc(strlen(part) + 1);
		strcpy(varg[i], part);
		part = strtok(NULL, " \t");
		++i;
	}

	if (! i)
		return;
	varg[i] = NULL;

	char* path = malloc(strlen(varg[0]) + 1);
	strcpy(path, varg[0]);

	if (! strcmp(path, CMD_CD)) {
		if (varg[1] != NULL)
			chdir(varg[1]);
	} else {
		char* name = strrchr(path, '/');
		if (name == NULL)
			name = path;
		else
			++name;
		varg[0] = name;

		printf("Path: %s\n", path);
		unsigned j = 0;
		while(varg[j]) {
			printf("arg #%d: %s\n", j, varg[j]);
			++j;
		}
/*
		pid_t pid = fork();
		if (pid < 0)
			return;
		else if (pid > 0) {
			echo_normal();
			int st;
			pid = wait(&st);
			echo_custom();
			if (st)
					; // normal
				else
					; // fail
		} else
			if (execvp(path, varg) == -1)
				exit(EXIT_FAILURE);
*/
	}

	free(path);
	while (--i)
		free(varg[i]);
}





