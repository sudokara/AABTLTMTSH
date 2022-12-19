#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "variables.h"

struct bgLL {
    pid_t pid;
    char *name;
    char status;
    int jobNum;
    struct bgLL *next;
};

extern int llsize, bgPcount, bgCompleted;

// forks a process and runs a process in the child. Parent does not wait for child.
// pid of child is added to a linked list of known running background processes
struct bgLL *makeBgProcess(struct bgLL *head, char **argv);

// part of service for handling SIGCHLD
// given the pid and error code, the number of the background process 
// is printed along with its pid and exit status
// the pid is removed from the list of bg processes
struct bgLL *handlePid(struct bgLL *head, pid_t pid, int errOccured);

// clears the linked list of background processes
void destroyAllPid(struct bgLL *head);

struct bgLL* insertPid(struct bgLL* head, pid_t pid, char *name, char status);

struct bgLL *removePid(struct bgLL *head, pid_t pid);