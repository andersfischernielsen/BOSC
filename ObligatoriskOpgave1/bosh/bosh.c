/* 

   bosh.c : BOSC shell 

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parser.h"
#include "print.h"

/* --- symbolic constants --- */
#define HOSTNAMEMAX 100

/* --- use the /proc filesystem to obtain the hostname --- */
char *gethostname(char *hostname)
{
  FILE *host = fopen("/proc/sys/kernel/hostname", "r");
  fscanf(host, "%s", hostname);
  close(host);
  return hostname;
}

/* --- execute a shell command --- */
int executeshellcmd (Shellcmd *shellcmd)
{ 
    Cmd *command = shellcmd->the_cmds;
    char** commands = command->cmd;          //Extract the first command from the user input.
    
    pid_t process_id1 = fork();              //Instantiate new process.
    
    if (process_id1 == 0) {                  //This is a child process.
        execvp(commands[0], commands);       //Execute.
    }
    
    else {
        int exit_code;
        waitpid(process_id1, &exit_code, 0);   //Assert that the process executed succesfully.
        
        if (exit_code != 0) {
            printf("Command not executed succesfully.");
        }
    }
    
    return 0;
}

/* --- main loop of the simple shell --- */
int main(int argc, char* argv[]) {

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

