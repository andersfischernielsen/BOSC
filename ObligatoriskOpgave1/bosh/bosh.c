/* 

   bosh.c : BOSC shell 

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
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

void startChild(Cmd *command, int readPipe, int writePipe, int first) {
    signal(SIGINT, SIG_DFL);                        //Enable CTRL+C to exit a program.

    if (command->next) {
        if (first) {
            close(readPipe);
            close(STDOUT_FILENUMBER);               // substitute stdout.
            dup(writePipe);
            close(writePipe);
        }
        else {                                      //If this is not the first command.
            close(STDIN_FILENUMBER);
            dup(readPipe);
            close(readPipe);

            close(STDOUT_FILENUMBER);
            dup(writePipe);
            close(writePipe);
        }
    }
    else {
        if (!first) {                               //This is the last command.
            close(writePipe);
            close(STDIN_FILENUMBER);                // substitute stdin.
            dup(readPipe);
            close(readPipe);
        } else {
            close(readPipe);
            close(writePipe);
        }
    }
    char** command_and_parameters = command->cmd;                   //Extract the first command from the user input.
    execvp(command_and_parameters[0], command_and_parameters);          //Execute.
    
    printf("%s: Command not found.\n", command_and_parameters[0]);      //If this code is run, it means command not found
    exit(-1);                                                           //Exit with error state.
}

/* --- execute a shell command --- */
int executeshellcmd (Shellcmd *shellcmd)
{ 
    int pipe_ends[2];
    pid_t process_id;
    Cmd *current_cmd = reverse(shellcmd->the_cmds);
    int first = 1;
    
    pipe(pipe_ends);

    while (current_cmd) {
        process_id = fork();                          //Instantiate new process.
        
        if (process_id == 0) {                              //This is a child process.
            startChild(current_cmd, pipe_ends[0], pipe_ends[1], first);
        }
        else {                                          //Parent process.
            first = 0;
            current_cmd = current_cmd->next;
        }
    }
    closePipe(pipe_ends);                               //Close the pipe in parent

    if (!shellcmd->background) {                //If the user specified to execute in background, then don't wait.
        int exit_code;
        waitpid(process_id, &exit_code, 0);     //Assert that the process executed succesfully.
    }

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
    printf("Exiting bosh.\n");
  }    
    
  return EXIT_SUCCESS;
}
