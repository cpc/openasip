/*
    Copyright 2002-2008 Tampere University.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
