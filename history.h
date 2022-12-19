#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variables.h"

struct historyStack{
    int top;
    int capacity;
    char **arr;
};

// make a stack of history commands
struct historyStack* makeHistoryStack(int capacity);

// add the command to the local history
void updateHistory(struct historyStack* history, char *command);

// get the history stored in the file .shhistory when starting up the shell
void fetchHistory(struct historyStack* history, char *pathToHistory);

// store the current state of the history in the file .shhistory
void storeHistory(struct historyStack* history, char *pathToHistory);

void printHistory(struct historyStack* history);