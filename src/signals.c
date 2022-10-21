#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <errno.h>

#include "signals.h"

void sigIntHandler(int signum, siginfo_t *sig, void *v) {
    //shd hv sth abt sigchild 
    //if parent hv child set a = false
    //if a false no printf("3230>")
}

void sigUsr1Handler(int signum, siginfo_t *sig, void *v) {
    setsigur1Received(true);
}

void handler() {
    struct sigaction sa_int, sa_usr;

    sigaction(SIGINT, NULL, &sa_int);
    sa_int.sa_flags = SA_SIGINFO;
    sa_int.sa_sigaction = sigIntHandler;
    sigaction(SIGINT, &sa_int, NULL);

    sigaction(SIGUSR1, NULL, &sa_usr);
    sa_usr.sa_sigaction = sigUsr1Handler;
    sigaction(SIGUSR1, &sa_usr, NULL);
}

void setsigur1Received(bool val) {
    sigur1Received = val;
}