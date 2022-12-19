#include "variables.h"

#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

// argv is a null terminated array of char pointers
// argv[0] must be the file to execute
// flags if any come next
// last element must be a NULL pointer
// returns time taken by the process in seconds.
long long createForegroundProcess(char **argv);

extern pid_t Pid;
extern int waitFg;