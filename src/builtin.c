/*!
   \file builtin.c
   \brief Builtin function implementations.
   \author WANG Zixu
   \date 20/10/2016

   \details
   Built-in functions such as 'exit' and 'timeX' are defined here and
   referenced as function pointers.
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include<sys/time.h>

#include "builtin.h"
#include "signals.h"

/*!
   \var const char* keywords[]
   \brief Keywords for builtin functions.

   \details
   Define keywords for search and mapping builtin functions.
*/
const char* keywords[] = {
  "exit",
  "cd",
  "timeX",
  0
};

/*!
   \var builtin_func_ptr builtins[]
   \brief Function pointers for builtin functions.

   \details
   Store function pointers for mapping.
*/
builtin_func_ptr builtins[] = {
  &builtin_exit,
  &builtin_cd,
  &builtin_timeX,
};

/*!
   \brief Map keywords to corresponding functions.

   \details
   This function takes the command name \p cmd and searches for builtin keyword.
   If \p cmd is found in the keywords, the corresponding function pointer in \c builtins[]
   is returned. Otherwise, \c NULL is returned.

   \param cmd The command string of the command line.

   \return The corresponding function pointer if \p cmd is a builtin keyword. Otherwise \c NULL.

   \code
   builtin_func_ptr myBuiltin = map(cmd);
   if (myBuiltin != NULL) {
     myBuiltin(argv, job); // call builtin.
   }
   \endcode
*/
builtin_func_ptr map(const char* cmd) {
  int i;
  for (i = 0; keywords[i] != NULL; ++i)
    if (strcmp(cmd, keywords[i]) == 0)
      return builtins[i];
  return NULL;
}

/*!
   \brief Builtin implementation for \c cd.

   \details
   Implementation of builtin function \c cd for changing directory.

   \param argv Command argument vector.
   \param j Pointer to the current job.

   \return Execution status.
*/
int builtin_cd(char** argv, Job* j) {
  int i, status;
  char dst[ARGV_SIZE] = {0};
  for (i = 1; argv[i] != NULL; ++i) {
    strcat(dst, argv[i]);
    strcat(dst, "/");
  }
  if (strcmp(dst, "") == 0)
    status = chdir(getenv("HOME"));
  else
    status = chdir(dst);
  if (status != 0) {
    perror("cd");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/*!
   \brief Builtin implementation for \c exit.

   \details
   Implementation of builtin function \c exit for exiting the shell.

   \param argv Command argument vector.
   \param j Pointer to the current job.

   \return Execution status.
*/
int builtin_exit(char** argv, Job* j) {
  // Check for following arguments.
  if (argv[1] != NULL || j->bg) {
    fprintf(stderr, "3230shell: \"exit\" with other arguments!!!\n");
    return EXIT_FAILURE;
  }
  else {
    printf("3230shell: Terminated.\n");
    exit(EXIT_SUCCESS);
  }
}

/*!
   \brief Builtin implementation for \c timeX.

   \details
   Implementation of builtin function \c timeX for timing a job.
   This function checks for eligibility of running timeX, and generates a new job
   from the rest part of the command line. The newly generated job is launched with a timer.

   \param argv Command argument vector.
   \param j Pointer to the current job.

   \return Execution status.
*/
int builtin_timeX(char** argv, Job* j) {
  // Check for background mode.
  if (j->bg) {
    fprintf(stderr, "3230shell: \"timeX\" cannot be run in background mode\n");
    return EXIT_FAILURE;
  }

  // Check for following arguments.
  if (argv[1] == NULL) {
    // If timeX is directly followed by a pipe.
    if (j->head->next != NULL)
      fprintf(stderr, "3230shell: Cannot timeX an incomplete '|' sequence\n");
    // If timeX has no arguments.
    else
      fprintf(stderr, "3230shell: \"timeX\" cannot be a standalone command\n");
    return EXIT_FAILURE;
  }

  // Emit 'timeX' from the new process argv.

  // Count for the number of arguments.
  size_t i;
  for (i = 1; argv[i]; i++);

  // Allocate memory for the new argv.
  char** cmdArgv = (char**)malloc((i+1)*sizeof(char*));
  if (cmdArgv == NULL) {
    fprintf(stderr, "3230shell: fail to allocate buffer.\n");
    return EXIT_FAILURE;
  }
  memset(cmdArgv, 0, (i+1)*sizeof(char*));

  // Copy rest of the command to the new argv.
  for (i = 0; argv[i+1]; i++)
    cmdArgv[i] = argv[i+1];
  cmdArgv[i+1] = NULL;
  free(argv);
  j->head->argv = cmdArgv;

  // Set flag for timeX running.
  isTimeX = 1;

  // Get start time stamp and launch the job.
  gettimeofday(&startTime, NULL);
  return launchJob(j);
}
