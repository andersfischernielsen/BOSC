/* 

   bosh.c : BOSC shell 

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "parser.h"
#include "print.h"

/* --- symbolic constants --- */
#define HOSTNAMEMAX 100

#define STDIN_FILENUMBER 0
#define STDOUT_FILENUMBER 1

/* --- use the /proc filesystem to obtain the hostname --- */
char *gethostname(char *hostname)
{
  FILE *host = fopen("/proc/sys/kernel/hostname", "r");
  fscanf(host, "%s", hostname);
  fclose(host);
  return hostname;
}

Cmd* reverse(Cmd* root) {
  Cmd* new_root = 0;
  while (root) {
    Cmd* next = root->next;
    root->next = new_root;
    new_root = root;
    root = next;
  }
  return new_root;
}

void start_child(Cmd *command, int readPipe, int writePipe) {
    signal(SIGINT, SIG_DFL);                        //Enable CTRL+C to exit a program.

    if (readPipe != STDIN_FILENUMBER) {
        dup2(readPipe, STDIN_FILENUMBER);
        close(readPipe);
    }
    if (writePipe != STDOUT_FILENUMBER) {
        dup2(writePipe, STDOUT_FILENUMBER);
        close(writePipe);
    }

    char** command_and_parameters = command->cmd;                   	//Extract the first command from the user input.
    execvp(command_and_parameters[0], command_and_parameters);          //Execute.

    printf("%s: Command not found.\n", command_and_parameters[0]);      //If these lines of code are run, it means command not found.
    exit(-1);                                                           //Exit with error state.
}

void set_in_out(Shellcmd *shellcmd, Cmd *current_cmd, int pipe_ends[], int pipe_in, int first, int *in, int *out) {
    close(pipe_ends[STDIN_FILENUMBER]); 					// Always close the in of the new pipe, as you should not read your own output.

    if (first && shellcmd->rd_stdin) {                                  // First with file redirect in.
        *in = open(shellcmd->rd_stdin, O_RDONLY);
        
        if (!(current_cmd->next)) {                                		// Only command.
            close(pipe_ends[STDOUT_FILENUMBER]);
            if (shellcmd->rd_stdout) {                                  // Only command with file redirect both in and out.
                *out = open(shellcmd->rd_stdout, O_WRONLY | O_CREAT, S_IRWXU);
            } else {                                                    // Only command with file redirect in.
                *out = STDOUT_FILENUMBER;
            }
        } else {                                                        // First (but not last command) with redirect in.
            *out = pipe_ends[STDOUT_FILENUMBER];
        }
    } else if (!(current_cmd->next) && shellcmd->rd_stdout) {           // Last (or only command) with file redirect out
        close(pipe_ends[STDOUT_FILENUMBER]);
        *in = pipe_in;
        *out = open(shellcmd->rd_stdout, O_WRONLY | O_CREAT, S_IRWXU);
    } else {                                                           	// No file redirect.
        if (first && !(current_cmd->next)) {                            // Only command.
            close(pipe_ends[STDOUT_FILENUMBER]);
            *in = pipe_in;
            *out = STDOUT_FILENUMBER;
        } else if (first) {                                             // First command.
            *in = pipe_in;
            *out = pipe_ends[STDOUT_FILENUMBER];
        } else if (!(current_cmd->next)) {                              // Last command.
            close(pipe_ends[STDOUT_FILENUMBER]);
            *in = pipe_in;
            *out = STDOUT_FILENUMBER;
        } else {                                                        // Middle command.
            *in = pipe_in;
            *out = pipe_ends[STDOUT_FILENUMBER];
        }
    }
}

/* --- execute a shell command --- */
int executeshellcmd (Shellcmd *shellcmd)
{ 
    int pipe_ends[2];
    int pipe_in = STDIN_FILENUMBER;                                    // Used for a future command, to read from a previous one.
    pid_t process_id;
    Cmd *current_cmd = reverse(shellcmd->the_cmds); //Commands are parsed back-to-front, so we reverse.
    
    int first = 1;                              //We're at the first command at first run.

    while (current_cmd) {
        if (pipe(pipe_ends) == -1) return -1;	//Try to pipe.

        process_id = fork();                   	//Instantiate new process.
        
        if (process_id == 0) {                 	//This is a child process.
            int *in, *out;
            *in = -1; *out = -1;
            set_in_out(shellcmd, current_cmd, pipe_ends, pipe_in, first, in, out);
            start_child(current_cmd, *in, *out);
        }
        else {                                  // Parent process.
            if (!first) {
                close(pipe_in);
            }
            pipe_in = pipe_ends[STDIN_FILENUMBER];
            close(pipe_ends[STDOUT_FILENUMBER]);
            first = 0;                              //No longer the first command.
            current_cmd = current_cmd->next;        //Assign the next command for execution.
        }
    }
    
    close(pipe_ends[STDIN_FILENUMBER]);         //Close the pipe in parent

    if (shellcmd->background) {                 //If the user specified to execute in background, then don't wait.
        return 0;
    }
    
    int exit_code;
    waitpid(process_id, &exit_code, 0);         //Assert that the process executed succesfully.

    return 0;
}

/* --- main loop of the simple shell --- */
int main(int argc, char* argv[]) {
  signal(SIGINT, SIG_IGN); // Ignore Ctrl + C - so we don't close the shell
  /* initialize the shell */
  char *cmdline;
  char hostname[HOSTNAMEMAX];
  int terminate = 0;
  Shellcmd shellcmd;
  
  if (gethostname(hostname)) {

    /* parse commands until exit or ctrl-c */
    while (!terminate) {
      printf("%s", hostname);
      if (strcmp(cmdline = readline(":# "), "exit") != 0) {
	if(*cmdline) {
	  add_history(cmdline);
	  if (parsecommand(cmdline, &shellcmd)) {
	    terminate = executeshellcmd(&shellcmd);
	  }
	}
	free(cmdline);
      } else terminate = 1;
    }
    printf("Exiting BOSH.\n");
  }    
    
  return EXIT_SUCCESS;
}
