/* 
    Implementation of process_cmd
    
    Student Name: YIU, Tung Hon
    ITSC email: thyiuaa@connect.ust.hk

    TODO: Please write the explanation here (i.e. near the top of the source code)
    
    For example:

    Step 1: Modify show_prompt() to display my ITSC username("thyiuaa")
    Step 2: Implement process_cmd()
        Step 2.1: tokenize the command received
            Step 2.1.1: create variables the store the string tokens
            Step 2.1.2: call tokenize() to retrieve tokenized command
        Step 2.2: 
    Step 3: ....

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


void process_cmd(char *cmdline)
{
    // TODO: write down your code for process_cmd
    char *pipe_segments[MAX_PIPE_SEGMENTS];
    int num_pipe_segments;
    tokenize(pipe_segments, cmdline, &num_pipe_segments, "|");

    int pfds[2];
    pipe(pfds);
    for (int current_segment = 0; current_segment < num_pipe_segments; ++current_segment) {
        pid_t pid = fork();
        if (pid == 0) { // child
            // pfds[0] and pfds[1] act as input and output alternately
            close(1);
            dup2(pfds[(current_segment+1)%2], 1);
            close(0);
            dup2(pfds[(current_segment%2], 0);

            // execute the command in current segment
            char *cmd[MAX_CMDLINE_LEN];
            int num_cmd;
            tokenize(cmd, pipe_segments[current_segment], num_cmd, "\t\r\n\v\f ");
            char *args[num_cmd];
            for (int i = 1; i < num_cmd; ++i) {
                args[i-1] = cmd[i];
            }
            args[num_cmd-1] = NULL;
            execvp(cmd[0], args);

            // terminate child precess
            exit();
        } else {
            wait(0);
        }
    }

    // Delete this line to start your work
    printf("Debug: %s\n", cmdline);
    
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
