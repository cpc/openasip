/**
 * @file exec_max_time.c
 *
 * Executes the given command for the given maximum time, and if the command
 * has not finished when the time runs out, kills the command and prints an
 * error message.
 *
 * SYNOPSIS
 * 
 * exec_max_time MAX_TIME COMMAND
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

/*#define DEBUG_OUTPUT*/

#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int child_pid;
void signal_handler(int data) {
    fprintf(stderr, "\nerror: timeout. Killing the process: ");
    fprintf(stderr, "SIGTERM ");
    kill(child_pid, SIGTERM);
    sleep(2);
    fprintf(stderr, "SIGINT ");
    kill(child_pid, SIGINT);
    sleep(2);
    fprintf(stderr, "SIGHUP ");
    kill(child_pid, SIGHUP);
    sleep(2);
    /* kill my child :,-( */
    fprintf(stderr, "SIGKILL\n");
    kill(child_pid, SIGKILL); 
    abort();
}

extern char **environ;
/**
 * Executes the given command in a shell, kills the process if
 * time runs out.
 *
 * @param command The shell command to execute.
 * @return Does not return if time ran out (aborts), the command's
 *         exit code otherwise.
 *
 */
int execute(int max_sec, char* command) {
    int exit_code = -1;
    int return_status = -1;
    int status;
    if (signal(SIGALRM, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Cannot initialize signal handler.");
        abort();
    }
    alarm(max_sec);    
    if ((child_pid = fork()) == 0) {
        char *argv[4];
        argv[0] = "bash";
        argv[1] = "-c";
        argv[2] = command;
        argv[3] = 0;
        execve("/bin/bash", argv, environ);
        exit(127);
    } 
    wait(&status);
    return status;
}

/**
 * Parses the command line arguments and starts the command.
 */
int main(int argc, char* argv[]) {
    
    int seconds = -1;

    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments.\n");
        return EXIT_FAILURE;	
    }

    if (sscanf(argv[1], "%d", &seconds) == 0 || seconds < 1) {
        fprintf(stderr, "Illegal count of seconds, must be > 0.\n");
        return EXIT_FAILURE;
    }
    
    return execute(seconds, argv[2]);
}
