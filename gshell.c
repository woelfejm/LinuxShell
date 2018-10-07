/*John Woelfel
Shell Lab
CS444 Spring 2018
5/11/2018
C compiled with gcc
sources consulted: https://brennan.io/2015/01/16/write-a-shell-in-c/
Description: This is a simple shell that reads in a string, parses it, then calls the appropriate function or gives the appropriate error. It additionally checks for & and handles it properly
Bugs: Sometimes the prompt icon '>' shows up before the child thread prints the output.
*/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"


int lsh_launch(char **args)
{
  //check if command ends in ampersand
  int amper = 0;
  int i = 0;
    while(args[i] != NULL){
	if(args[i][0] == '&' && args[i][1] == '\0'){
		//printf("end with ampersand");
		args[i] = '\0';  //remove ampersand from args
		amper = 1;  //set amper to true
        }
	i++;
    }


  pid_t pid, wpid;

  int status;
  //Part B: Allow ampersand
  //don't wait for child to return
  if(amper == 1){

	pid = fork();
	if ( pid == 0 ) { // child process
		execvp(args[0], args) ;
	}
	else { // parent
		//don't wait for child
		printf("Child ID:%d\n",pid); //print child ID
	}
	return 1;
  }
  else{
	  pid = fork();
	  if (pid == 0) {
	    // Child process
	    if (execvp(args[0], args) == -1) {
	      perror("lsh");
	    }
	    exit(EXIT_FAILURE);
	  } else if (pid < 0) {
	    // Error forking
	    perror("lsh");
	  } else {
	    // Parent process
	    do {
	      wpid = waitpid(pid, &status, WUNTRACED);
	    } while (!WIFEXITED(status) && !WIFSIGNALED(status)); //wait for child to return
	  }
  return 1;
  }

}

char *lsh_read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  getline(&line, &bufsize, stdin);
  return line;
}

char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();

    //Phase 1: Echo command to standard out
    //printf("%s", line);   //Phase 1 test

    //Phase 2: Parse command into tokens
    args = lsh_split_line(line);

    //Phase 2 test
    /*
    int i = 0;
    while(args[i] != NULL){
	printf("%s\n",args[i]);
	i++;
    }
    */

    //Phase 3 and 4
    status = lsh_launch(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
