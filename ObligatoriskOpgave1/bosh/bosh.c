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

void startChild(Cmd *command, int readPipe, int writePipe, int first) {
    signal(SIGINT, SIG_DFL);                        //Enable CTRL+C to exit a program.

    if (command->next) {							//This is one of several commands.
        if (first) {
            close(readPipe);						//Close the pipe end we don't need.
            dup2(writePipe, STDOUT_FILENUMBER);     //Substitute stdout with pipe.
            close(writePipe);						//Close after "assigning" pipe end.
        }
        else {                                      //"Middle" command.
            dup2(readPipe, STDIN_FILENUMBER);
            close(readPipe);

            dup2(writePipe, STDOUT_FILENUMBER);
            close(writePipe);
        }
    }
    else {											//This is the last or a single command.
        if (!first) {                               //This is the last command.
            close(writePipe);
            dup2(readPipe, STDIN_FILENUMBER);       //Substitute stdin as above, now just stdin.
            close(readPipe);
        } else {                                    //Single command.
            close(readPipe);
            close(writePipe);
        }
    }

    char** command_and_parameters = command->cmd;                   	//Extract the first command from the user input.
    execvp(command_and_parameters[0], command_and_parameters);          //Execute.

    printf("%s: Command not found.\n", command_and_parameters[0]);      //If these lines of code are run, it means command not found.
    exit(-1);                                                           //Exit with error state.
}

/* --- execute a shell command --- */
int executeshellcmd (Shellcmd *shellcmd)
{ 
    int pipe_ends[2];
    int pipe_in;                                    // Used for a future command, to read from a previous one.
    pid_t process_id;
    Cmd *current_cmd = reverse(shellcmd->the_cmds); //Commands are parsed back-to-front, so we reverse.
    
    int first = 1;                              //We're at the first command at first run.

    while (current_cmd) {
        if (!first) {
            close(pipe_ends[STDOUT_FILENUMBER]);
        }
        if (pipe(pipe_ends) == -1) return -1;

        process_id = fork();                   	//Instantiate new process.
        
        if (process_id == 0) {                 	//This is a child process.
            startChild(current_cmd, pipe_in, pipe_ends[STDOUT_FILENUMBER], first);
        }
        else {                                  // Parent process.
            if (!first) {
                close(pipe_in);
            }
            pipe_in = pipe_ends[STDIN_FILENUMBER];
            first = 0;                              //No longer the first command.
            current_cmd = current_cmd->next;        //Assign the next command for execution.
        }
    }
    
    close(pipe_ends[STDIN_FILENUMBER]);         //Close the pipe in parent
    close(pipe_ends[STDOUT_FILENUMBER]);

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
    printf("Exiting bosh.\n");
  }    
    
  return EXIT_SUCCESS;
}
