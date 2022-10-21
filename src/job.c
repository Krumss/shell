#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <sys/wait.h>
#include <string.h>

#include "job.h"
#include "signals.h"
#include "constants.h"
#include "builtin.h"

int launchJob(Job *j) {
    if (j == NULL)
        return -1;
    signal(SIGINT, SIG_IGN);

    setsigur1Received(false);

    Process* p = j->head;
    pid_t pid;
    int status = EXIT_SUCCESS;
    if (p->next == NULL) {
        if (!isTimeX) {
            // Check if the command is a builtin keyword.
            builtin_func_ptr builtin = map(p->argv[0]);
            if (builtin != NULL)
                // Invoke builtin function if is builtin command.
                return builtin(p->argv, j);
        }

        pid = fork();
        if (pid == 0) {
            /*while (!sigur1Received) {}
            sigur1Received = 0;
            printf("HIHI %s\n", p->argv[0]);
            execvp(p->argv[0], p->argv);*/
            launchPipeProcess(p, j->stdin, j->stdout);
        }
        else if(pid < 0) {
            printf("fork: error no = %s\n", strerror(errno));
            exit(-1);
        }
        else {
            p->pid = pid;
            pid_t child_pid;
            int status;
            kill(pid, SIGUSR1);
            //child_pid = waitpid(pid, &status, 0);
            if (isTimeX)
                // If it's a timeX job, hold the terminated process
                // in zombie state for further handling.
                waitid(P_PID, pid, NULL, WEXITED | WNOWAIT);
            else
                // Wait for child process if in foreground.
                waitpid(pid, &status, j->bg);

            //printf("Child process (%d) exited, with exit status %d\n", (int) child_pid, WEXITSTATUS(status));
        }
        
    } else {
        int in, out, pos;
        in = j->stdin;
        pos = 0;
        for (; p; p = p->next) {
            if (p->next) {
                pipe(j->pipes[pos]);
                out = j->pipes[pos][1];
            } else {
                out = j->stdout;

                //check if command is builtin keyword.
            }
            builtin_func_ptr builtin = map(p->argv[0]);
            if (builtin != NULL) {
                pid = fork();
                if (pid == 0) {
                    exit(builtin(p->argv, j));
                } else if (pid < 0) {
                    // Error when forking.
                    error(EXIT_FAILURE, errno, "fork");
                } else {
                    // In parent process, set process id
                    // and send SIGUSR1 to let child process run.
                    p->pid = pid;
                    kill(pid, SIGUSR1);
                }
            } else { //TODO: check if is input command
                pid = fork();
                if (pid == 0) {
                    launchPipeProcess(p, in, out);
                } else if (pid < 0) {
                    error(EXIT_FAILURE, errno, "fork");
                } else {
                    p->pid = pid;

                    // sleep for 15000 microseconds to avoid race condition
                    // between processes in pipe. Then send SIGUSR1.
                    usleep(15000);
                    kill(pid, SIGUSR1);
                }          
                
            }
            if (in != j->stdin)
                close(in);
            if (out != j->stdout)
                close(out);
            in = j->pipes[pos][0];

            pos++;
        }
        for (p = j->head; p; p = p->next) {
            if (p->pid >= 0) {
                /*if (isTimeX)
                // If it's a timeX job, hold the terminated processes
                // in zombie state for further handling.
                waitid(P_PID, p->pid, NULL, WEXITED | WNOWAIT);
                else*/
                waitpid(p->pid, &status, j->bg);
            }
        }
    }
    cleanJob(j);

    return WEXITSTATUS(status);
}

void launchPipeProcess(Process* p, int in, int out) {
    // If input is redirected, alter file descriptor.
    if (in != STDIN_FILENO) {
        dup2(in, STDIN_FILENO);
        close(in);
    }

    // If output is redirected, alter file descriptor.
    if (out != STDOUT_FILENO) {
        dup2(out, STDOUT_FILENO);
        close(out);
    }

    // Set the group process id.
    // Group process id is set to pid of p if pgid equals to 0.
    // This way we can deal with background processes separately.
    //setpgid(0, pgid);

    // Block untill SIGUSR1 has been received.
    while (!sigur1Received)
    ;
    sigur1Received = 0;

    // Invoke the command. Prompt error message and exit if there's an error.
    if (execvp(p->argv[0], p->argv) == -1)
        error(EXIT_FAILURE, errno, "'%s'", p->argv[0]);
    exit(EXIT_FAILURE);
}

/*!
   \brief Clean up a \link Job job \endlink.

   \details
   This function releases the memory of job \p j and reset flag \c isTimeX.
   It releases the the process(es) and then releases the job itself.

   \param j Pointer to the job to be cleaned.

   \sa cleanProcess
*/
void cleanJob(Job* j) {
  //isTimeX = 0;
  if (j == NULL)
    return;
  cleanProcess(j->head);
  free(j);
}

/*!
   \brief Clean up a \link Process process \endlink.

   \details
   This function releases the memory of process \p p recursively.
   It releases the argument vector and then releases the process itself.

   \param p Pointer to the process to be cleaned.
*/
void cleanProcess(Process* p) {
  if (p == NULL)
    return;
  cleanProcess(p->next);
  if (p->argv != NULL)
    free(p->argv);
  free(p);
}