#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <string.h>

#include "variables.h"

// calls the correct function according to flags given
void listDirCaller(int allDisplay, int longDisplay, int argc, char *argv[], int idx);