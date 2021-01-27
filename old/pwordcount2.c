#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 1
#define READ_END	0
#define WRITE_END	1

int readfromfile(FILE *fp, int writepipe[])
int wordcount(int readpipe[]);


int main(int argc, char *argv[])
{
	pid_t child_pid, wpid;
	int status = 0;
	int tochild[2];
	int fromchild[2];
	int childcount;
	int totalcount = 0;
	FILE *fp;

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

	if (pid < 0) {
		fprintf(stderr, "Fork failed");
		return 1;
	}

	if (pid > 0) {  /* parent process */
		/* close the unused end of the pipes */
		close(tochild[READ_END]);
		close(fromchild[WRITE_END]);
/* read from file, write it to the pipe, wait for result */
		if (argc == 1)
			printf("usage pwordcount <filename>");
		if ((fp = fopen(argv[1], "r")) == NULL) {
			printf("wordcount: can't open %s\n", argv[1]);
			return 1;
		} else {
			readfromfile(*fp, tochild[]);
			fclose(fp);
		}
		while ((wpid = wait(&status)) > 0); // this way, the father waits for all the child processes

		/* do things after all child processes done */
		read(fromchild[READ_END], childcount, BUFFER_SIZE);
		totalcount += childcount;
                close(fromchild[READ_END]);
                close(tochild[WRITE_END]);
	}

	else {  /* child process */
                /* close the unused end of the pipes */
                close(fromchild[READ_END]);
                close(tochild[WRITE_END]);
/* read from pipe, count the words, send result back to pipe */
		write(fromchild[WRITE_END], wordcount(tochild[]), BUFFER_SIZE);
	        close(tochild[READ_END]);
                close(fromchild[WRITE_END]);
        }
return 0;
}

int readfromfile(FILE *fp, int writepipe[])
{
	/* read from a file and write it to a pipe */
	int c;
	while ((c = getc(fp)) != EOF)
		putc(c, writepipe[WRITE_END]);
	putc(c, EOF);
	return 0;
}


int wordcount(int readpipe[])
{
/* read characters from a pipe and count the words */
/* wordcount routine from Kernigan-Ritchie chapter 1 */

#define IN 1 	/* inside a word */
#define OUT 0 	/* outside a word */


	int c, nw, state;

	state = OUT;
	nw = 0;
	while ((read(readpipe[READ_END], c, BUFFER_SIZE) != EOF) {
		if (c == ' ' || c == '\n' || c == '\t')
			state = OUT;
		else if (state == OUT) {
			state = IN;
			++nw;
		}
	}
	printf("%d\n", nw);
	return nw;
}

