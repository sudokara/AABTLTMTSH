#include "variables.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

extern int handleInput(char *inp, int fg);

int handlePipe(char *cmnd);