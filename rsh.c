#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12

extern char **environ;

char *allowed[N] = {"cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff","cd","exit","help"};

int isAllowed(const char*cmd) {
	// TODO
	// return 1 if cmd is one of the allowed commands
	// return 0 otherwise
	
	int validCommand = 0;

	for (int i = 0; i < 12; i++) {
		if (strcmp(cmd, allowed[i]) == 0) {
			validCommand = 1;
		}
	}


	return validCommand;
}

int main() {

    // TODO
    // Add variables as needed
	posix_spawnattr_t attr;

    // Initialize spawn attributes
    posix_spawnattr_init(&attr);

	int status;
	pid_t pid;

    char line[256];

    while (1) {

		fprintf(stderr,"rsh> ");

		if (fgets(line,256,stdin)==NULL) continue;

		if (strcmp(line,"\n")==0) continue;

		line[strlen(line)-1]='\0';

		// TODO
		// Add code to spawn processes for the first 9 commands
		// And add code to execute cd, exit, help commands
		// Use the example provided in myspawn.c

		char *cmd = strtok(line, " ");


		if (cmd == NULL) {
			continue;
		}

		
		if(isAllowed(cmd) == 0) {
			printf("NOT ALLOWED!\n");
			continue;
		}

		// Set flags if needed, for example, to specify the scheduling policy
		// posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSCHEDULER);

		if (strcmp(cmd, "cd") == 0) {

			char *dir = strtok(NULL, " ");
			if (dir == NULL) {
				fprintf(stderr, "-rsh: cd: missing argument\n");
			} else if (strtok(NULL, " ") != NULL) {
				fprintf(stderr, "-rsh: cd: too many arguments\n");
			} else if (chdir(dir) != 0) {
				perror("-rsh: cd failed");
			}
			continue;

		} else if ( strcmp(cmd, "exit") == 0 ) {
			break;

		} else if (strcmp(cmd, "help") == 0) {

			printf("The allowed commands are:\n");
			for (int i = 0; i < N; i++) {
				printf("%i: %s\n", (i + 1), allowed[i]);
			}
			
			continue;
		}


		char *arg[20];
		arg[0] = cmd;
		int count = 1;

        char *argument;

        while ((argument = strtok(NULL, " ")) != NULL) {
            arg[count++] = argument;
        }
        arg[count] = NULL;

		// Initialize spawn attributes
		posix_spawnattr_init(&attr);

		// Set flags if needed, for example, to specify the scheduling policy
		// posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSCHEDULER);

		// Spawn a new process
		if (posix_spawnp(&pid, cmd, NULL, &attr, arg, environ) != 0) {
			perror("spawn failed");
			exit(EXIT_FAILURE);
		}

		// Wait for the spawned process to terminate
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}

		if (WIFEXITED(status)) {
			fprintf(stderr, "Spawned process exited with status %d\n", WEXITSTATUS(status));
		}

		// Destroy spawn attributes
		posix_spawnattr_destroy(&attr);

    }
    return 0;
}
