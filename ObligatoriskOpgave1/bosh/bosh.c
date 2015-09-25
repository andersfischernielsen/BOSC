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

/* --- execute a shell command --- */
int executeshellcmd (Shellcmd *shellcmd)
{ 
    Cmd *current_cmd = shellcmd->the_cmds;
    int first = 1;
    int pipe_ends[2];
    pipe(pipe_ends);                                    //TODO: Error handling if piping fails.

    while (current_cmd) {
        pid_t process_id = fork();                      //Instantiate new process.
        
        if (process_id == 0) {                          //This is a child process.
            signal(SIGINT, SIG_DFL);                    //Enable CTRL+C to exit a program.
            
            if (current_cmd->next) {
                if (first) {
                    first = 0;                              // The cmd can no longer be the first.
                    close(pipe_ends[STDIN_FILENUMBER]);
                }
                else {                                      //If this is not the first command.
                    close(STDIN_FILENUMBER);                //Close stdin for the process.
                    dup(pipe_ends[STDIN_FILENUMBER]);       //Set the process stdin to the pipes read end.
                }
                
                close(STDOUT_FILENUMBER);                   //Always set the stdout for the process to the pipe write end.
                dup(pipe_ends[STDOUT_FILENUMBER]);
            }
            
            else {                                          //This is the last (or the only) command.
                if (!first) {
                    close(STDIN_FILENUMBER);                //Don't dup the write end of the pipe to stdout of the process.
                    dup(pipe_ends[STDIN_FILENUMBER]);           
                }
                else {                                      //If we only have one command.
                    close(pipe_ends[STDIN_FILENUMBER]);
                }
                
                close(pipe_ends[STDOUT_FILENUMBER]);
            }
            
            char** command_and_parameters = current_cmd->cmd;                   //Extract the first command from the user input.
            execvp(command_and_parameters[0], command_and_parameters);          //Execute.
            
            printf("%s: Command not found.\n", command_and_parameters[0]);      //If this code is run, it means command not found
            exit(-1);                                                           //Exit with error state.
            return;
        }
        
        else {                                          //Parent process.
            current_cmd = current_cmd->next;
            
            if (!shellcmd->background) {                //If the user specified to execute in bacground, then don't wait.
                int exit_code;
                waitpid(process_id, &exit_code, 0);     //Assert that the process executed succesfully.
            }
        }
    }
    
    close(pipe_ends[STDIN_FILENUMBER]);                 //Close the pipe.
    close(pipe_ends[STDOUT_FILENUMBER]);

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
