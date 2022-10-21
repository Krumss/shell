#pragma once

#include <signal.h>
#include <stdbool.h>
#include "sys/wait.h"


bool sigur1Received;
volatile sig_atomic_t isTimeX;
/*!
   \var struct timeval startTime
   \brief Start time stamp.

   \details
   Record start time stamp for \c timeX jobs to calculate real time.
*/
struct timeval startTime,

/*!
   \var struct timeval endTime
   \brief End time stamp.

   \details
   Record end time stamp for \c timeX jobs to calculate real time.
*/
               endTime;

void sigIntHandler(int signum, siginfo_t *sig, void *v);
void sigUsr1Handler(int signum, siginfo_t *sig, void *v);

void handler();

void setsigur1Received(bool val);

