#include "variables.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

/* typedef struct job{
    int jobNum;
    pid_t pid;
    char *cmnd;
    char state;
    struct job* next;
} Job;

extern Job *jobList;
extern int jobCount;

void jobs(Job *jobList, int running, int stopped);

void sig(Job *jobList, int signum, int jobNum);

void fg(Job *jobList, int jobNum);

void bg(Job *jobList, int jobNum); */
extern pid_t Pid;

void jobs(struct bgLL *head, int running, int stopped);

void sig(struct bgLL *head, int signum, int jobNum);

struct bgLL *fgFn(struct bgLL *head, int jobNum);

void bgFn(struct bgLL *head, int jobNum);