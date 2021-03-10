/* 
    Implementation of process_cmd
    
    Student Name: YIU, Tung Hon
    ITSC email: thyiuaa@connect.ust.hk

    Step 1: Modify show_prompt() to display my ITSC username("thyiuaa")
    Step 2: tokenize the command received into segments
        Step 2.1: create variables to store the segments and the total number of segments
        Step 2.2: call tokenize() to retrieve segments that separate by "|"
    Step 3: check if there is exit command
    	Step 3.1: loop through all segments
    	Step 3.2: show teminate message and terminate if there is a exit command
    Step 4: execute the piped commands
    	Step 4.1: set read end for the first segment to 0 (stdin)
    	Step 4.2: loop through all segment
    	Step 4.3: create pipe and fork
    	Step 4.4: if is child, divide segment into different groups
			Step 4.4.1: do nothing to the file descriptor if there is only 1 segment
			Step 4.4.2: set output to pipe if the segment is the first
			Step 4.4.3: set input to the read end if the segment is the last
			Step 4.4.4: set input and output to pipe if the segment is in middle
    	Step 4.5: if is child, run the segment command
    		Step 4.5.1: tokenise the segment command with "\t\r\n\v\f " as delimiter
    		Step 4.5.2: use execvp() to load program of the command into the process for execution
    	Step 4.6: if is parent, close write end of the pipe and wait for child
    	Step 4.7: if is parent, return the read end of the current pipe and go to Step 4.3 until no segments left

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LEN 256

// Assume that we have at most 16 pipe segments
#define MAX_PIPE_SEGMENTS 16

// Assume that each segment has at most 256 characters (including NULL)
#define MAX_SEGMENT_LENGTH 256
 
// This function will be invoked by main()
// TODO: Implement the multi-level pipes below
void process_cmd(char *cmdline);

// This function will be invoked by main()
void show_prompt();


// This function will be invoked by main()
// The implementation is given
int get_cmd_line(char *cmdline);

// tokenize function is given
// This function helps you parse the command line
//
// Suppose the following variables are defined:
//
// char *pipe_segments[MAX_PIPE_SEGMENTS]; // character array buffer to store the pipe segements
// int num_pipe_segments; // an output integer to store the number of pipe segment parsed by this function
// char cmdline[MAX_CMDLINE_LEN]; // The input command line
//
// Sample usage of this tokenize function:
//
//  tokenize(pipe_segments, cmdline, &num_pipe_segments, "|");
// 
void tokenize(char **argv, char *line, int *numTokens, char *token);


/* The main function implementation */
int main()
{
    char cmdline[MAX_CMDLINE_LEN];
    printf("The shell program (pid=%d) starts\n", getpid());
    while (1)
    {
        show_prompt();
        if (get_cmd_line(cmdline) == -1)
            continue; /* empty line handling */

        process_cmd(cmdline);
    }
    return 0;
}

void run_cmd(char segment[]) {
    char *cmd[MAX_CMDLINE_LEN];
    int num_cmd;
    tokenize(cmd, segment, &num_cmd, "\t\r\n\v\f ");
    execvp(cmd[0], cmd);
}

void check_exit(int total, char* pipes[]) {
	char exit_str[MAX_CMDLINE_LEN] = "exit";
	for (int current = 0; current < total; ++current) {
		char cmd_str[MAX_CMDLINE_LEN];
		strcpy(cmd_str, pipes[current]);
		if (strcmp(strtok(cmd_str, "\t\r\n\v\f "), exit_str) == 0) {
	    	printf("The shell program (pid=%d) terminates\n", getpid());
	        exit(0);
    	}
    }
}

int execute_pipe_segment(char *pipes[], int total, int current, int read_end) {
	int pfds[2]; // 0: read-end, 1: write-end
	pipe(pfds);
	pid_t pid = fork();
    if (pid == 0) {
        if (total == 1) {
			// do nothing to file descriptor
        } else if (current == 0) {
        	close(1);
        	dup(pfds[1]);
        	close(pfds[0]);
        } else if (current == total - 1) {
            close(0);
            dup(read_end);
            close(pfds[0]);
            close(pfds[1]);
        } else {
        	close(0);
            dup(read_end);
            close(1);
        	dup(pfds[1]);
        }
        run_cmd(pipes[current]);
    } else {
    	close(pfds[1]);
        wait(0);
        return pfds[0];
    }
}

void process_cmd(char *cmdline)
{
    char *pipes[MAX_PIPE_SEGMENTS];
    int total;
    tokenize(pipes, cmdline, &total, "|");
    check_exit(total, pipes);
	int read_end = 0; // stdin
    for (int current = 0; current < total; ++current) {
    	read_end = execute_pipe_segment(pipes, total, current, read_end);
    }
}

void show_prompt()
{
    printf("$thyiuaa> ");
}

int get_cmd_line(char *cmdline)
{
    int i;
    int n;
    if (!fgets(cmdline, MAX_CMDLINE_LEN, stdin))
        return -1;
    // Ignore the newline character
    n = strlen(cmdline);
    cmdline[--n] = '\0';
    i = 0;
    while (i < n && cmdline[i] == ' ') {
        ++i;
    }
    if (i == n) {
        // Empty command
        return -1;
    }
    return 0;
}

// Implementation of tokenize function
void tokenize(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    argv[argc++] = NULL;
    *numTokens = argc - 1;
}
