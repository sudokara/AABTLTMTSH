#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "variables.h"

// Returns a pointer to a string of a PATH_MAX byte buffer.
//! Always free the returned value after use
char *getCurrentDirectory();

// Get current working directory and print it to stdout.
// Automatically mallocs and frees buffer
void printCurrentDirectory();