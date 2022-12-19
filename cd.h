#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "variables.h"
#include "prompt.h"
#include "pwd.h"

// Takes a relative path and returns an absolute path
char *getAbsPath(char *path);

// Changes directory to path and returns path
// returns NULL on failure
char *changeDirect(char *path);

// Changes directory and updates prerfix
// returns prefix
char *changeDirectoryUpdatePrefix(char *prefix, char *path);