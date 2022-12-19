#include "variables.h"
#include "pwd.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/utsname.h>  
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
// Puts prompt to prefix and returns prefix
// update specifies which part of the prompt must be updated
// update & 1 : username
// update & 2 : sysname
// update & 4 : currdir
char *getPrefix(char *prefix, int update);