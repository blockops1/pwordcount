#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define BUFFER_SIZE 4
#define READ_END	0
#define WRITE_END	1
#define IN 1 	/* inside a word */
#define OUT 0 	/* outside a word */

int main(int argc, char *argv[])
{
	/* Program to perform a word count of a file */
	/* wordcount routine from Kernigan-Ritchie chapter 1 */
	/* the idea is to have the main process read from the file */ 
	/* while the child process does the word counting */
	/* The buffer matches the size of a single character*/

	int d, nw, state;
	pid_t child_pid, wpid;
	int status = 0;
	int tochild[2]; // pipe from main process to child
	int fromchild[2]; // pipe from child process to main
	int childcount;
	int totalcount = 0;
	FILE *fp;
	int c;

	/* create the pipe to the child*/
	if (pipe(tochild) == -1) {
		fprintf(stderr,"Pipe failed");
		return 1;
	}
	
	/* create the pipe from the child*/
	if (pipe(fromchild) == -1) {
		fprintf(stderr,"Pipe failed");
		return 1;
	}

	/* now fork a child process */
	
	child_pid = fork();

	if (child_pid < 0) {
		fprintf(stderr, "Fork failed");
		return 1;
	}

	if (child_pid > 0) {  /* parent process */
		/* close the unused end of the pipes */
		close(tochild[READ_END]);
		close(fromchild[WRITE_END]);
		/* read from file, write it to the pipe, wait for result */
		if (argc == 1) {
			printf("usage is with an argument, like:\npwordcount <filename>\n");
			return 1;
		}
		if ((fp = fopen(argv[1], "r")) == NULL) {
			printf("pwordcount: can't open the file with name %s\n", argv[1]);
			return 1;
		} else {
		/* read from a file and write it to a pipe */
			while ((c = getc(fp)) != EOF) {
				write(tochild[WRITE_END], (void*)&c, BUFFER_SIZE);
			}
			c = 0;
			write(tochild[WRITE_END], (void*)&c, BUFFER_SIZE);
			fclose(fp);
			close(tochild[WRITE_END]);
		}
		while ((wpid = wait(&status)) > 0); // this way, the parent waits for all the child processes

		/* do things after all child processes done */
		read(fromchild[READ_END], (void*)&childcount, BUFFER_SIZE);
		totalcount += childcount;
        close(fromchild[READ_END]);
		printf("word count of file %s: %i\n", argv[1], totalcount);
        /* close(tochild[WRITE_END]); */
	}

	else {  /* child process */
        /* close the unused end of the pipes */
        close(fromchild[READ_END]);
        close(tochild[WRITE_END]);
		/* read from pipe, count the words, send result back to pipe */
		state = OUT;
		nw = 0;
		while (read(tochild[READ_END], (void*)&d, BUFFER_SIZE) != 0) {
			if (d == ' ' || d == '\n' || d == '\t')
				state = OUT;
			else if (state == OUT) {
				state = IN;
				++nw;
			}
		}
		/* subtract 1 from nw because of 0 at end of pipe */
		nw--;
		write(fromchild[WRITE_END], (void*)&nw, BUFFER_SIZE);
	    close(tochild[READ_END]);
        close(fromchild[WRITE_END]);
		exit(0);
    }
return 0;
}