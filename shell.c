#include "stdio.h"
#include "stdlib.h"
#include "string.h" //strtok
#include "sys/wait.h"
#include "unistd.h"
#include <errno.h>

#define MAX_SIZE_CMD        1024
#define MAX_STRING_LENGTH   30

char cmd[MAX_SIZE_CMD];				    // string holder for the command
char *argv[MAX_STRING_LENGTH];			// an array for command and arguments
pid_t pid;

void c_shell();
void get_cmd();
void cmd_to_argv();
void handler();
void handleExit(char* arg);

void sigIntHandler(int signum, siginfo_t *sig, void *v);
void sigKillHandler(int signum, siginfo_t *sig, void *v);

int main() {
    c_shell();
    return 0;
}

void c_shell() {
    printf("Shell PID: %d\n", (int)getpid());
    while(1) {
            struct sigaction sa_int, sa_kill;

    sigaction(SIGINT, NULL, &sa_int);
    sa_int.sa_flags = SA_SIGINFO;
    sa_int.sa_sigaction = sigIntHandler;
    sigaction(SIGINT, &sa_int, NULL);

        get_cmd();

        cmd_to_argv();

        //handleExit(argv[0]);

        pid = fork();

        if(pid == 0) {  //child
            execvp(argv[0], argv);
        }
        else if(pid < 0) {
            printf("fork: error no = %s\n", strerror(errno));
            exit(-1);
        }
        else {
            pid_t child_pid;
            int status;
            child_pid = waitpid(pid, &status, 0);

            printf("Child process (%d) exited, with exit status %d\n", (int) child_pid, WEXITSTATUS(status));
        }

    }
}

void get_cmd() {
	// get command from user
    printf("$$ 3230shell ##   ");
	fgets(cmd, MAX_SIZE_CMD, stdin);
	// remove trailing newline
	if ((strlen(cmd) > 0) && (cmd[strlen (cmd) - 1] == '\n'))
        cmd[strlen (cmd) - 1] = '\0';
}

void cmd_to_argv() {
    char *arg_ptr;
    int i = 0;
    arg_ptr = strtok(cmd, " ");
    while(arg_ptr != NULL) {
        argv[i] = arg_ptr;
        i++;
        arg_ptr = strtok(NULL, " ");
    }
    //handleExit(argv[0]);
}

void handler() {
    struct sigaction sa_int, sa_kill;

    sigaction(SIGINT, NULL, &sa_int);
    sa_int.sa_flags = SA_SIGINFO;
    sa_int.sa_sigaction = sigIntHandler;
    sigaction(SIGINT, &sa_int, NULL);

    sigaction(SIGKILL, NULL, &sa_kill);
    sa_kill.sa_flags = SA_SIGINFO;
    sa_kill.sa_sigaction = sigKillHandler;
    sigaction(SIGKILL, &sa_kill, NULL);
}

void sigIntHandler(int signum, siginfo_t *sig, void *v) {
    printf("This process is Interrupt\n");
}

void sigKillHandler(int signum, siginfo_t *sig, void *v) {
    printf("This process is Killed\n");
}

void handleExit(char* arg) { //segmentation fault becoz we put handleExit before typing, or we use control c.
    if(strcmp(arg, (char*)"exit") == 0) {
        printf("3230shell: Exit\n");
        exit(0);
    }
}