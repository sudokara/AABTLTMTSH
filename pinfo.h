#include "variables.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// prints the pinfo of the current process
// finds its own pid and calls printPinfoPid
void printPinfoCurrent();

// given a pid, it reads /proc/pid/stat and prints relevant info
void printPinfoPid(pid_t pid);