#include "variables.h"

#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// calls appropriate function given the arguments
void discover(int argc, char *argv[], int index, int files, int directories);