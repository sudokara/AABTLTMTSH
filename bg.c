#include "bg.h"

int llsize = 0;
int bgPcount = 0;
int bgCompleted = 0;

struct bgLL* insertPid(struct bgLL* head, pid_t pid, char *name, char status)
{
    if (head == NULL) // first bg process
    {
        head = malloc(sizeof(struct bgLL));
        head->next = NULL;
        head->pid = pid;
        head->status = status;
        head->jobNum = ++bgPcount;
        head->name = malloc(strlen(name) + 1);
        strcpy(head->name, name);
    }
    else // traverse list and insert pid
    {
        struct bgLL *temp;
        for (temp = head; temp->next != NULL; temp = temp->next);
        temp->next = malloc(sizeof(struct bgLL));
        temp->next->next = NULL;
        temp->next->pid = pid;
        temp->next->status = status;
        temp->next->jobNum = ++bgPcount;
        temp->next->name = malloc(strlen(name) + 1);
        strcpy(temp->next->name, name);
    }

    llsize++;
    return head;
}

struct bgLL *removePid(struct bgLL *head, pid_t pid)
{
    if (!head) // no pids found
        return head;

    if (head->pid == pid) // first pid
    {
        struct bgLL *temp = head->next;
        free(head->name);
        free(head);
        llsize--;
        bgCompleted++;
        return temp;
    }

    struct bgLL *temp = head, *prev = head;
    for (; temp->pid != pid; prev = temp, temp = temp->next);
    prev->next = temp->next;
    free(temp->name);
    free(temp);

    bgCompleted++;
    llsize--;
    return head;
}

void destroyAllPid(struct bgLL *head)
{
    for (struct bgLL *temp = head; temp != NULL;)
    {
        head = temp;
        temp = temp->next;
        free(head);
    }
    llsize = 0;
}

struct bgLL* handlePid(struct bgLL *head, pid_t pid, int errOccured)
{
    if (!head) // no pids found
        return head;

    if (head->pid == pid) // first pid
    {
        if (!errOccured)
        {
            // fprintf(stderr, "\033[2K");
            fprintf(stderr,"\r%s with pid = %d exited normally\n %s ", head->name, head->pid, prefix);
        }
        else
        {
            // fprintf(stderr, "\033[2K");
            fprintf(stderr, "\r%s with pid = %d exited with exit code %d\n %s ", head->name, head->pid, errOccured,prefix);
        }
        fflush(stderr);
        head = removePid(head, pid);
        return head;
    }

    struct bgLL *temp = head;
    int i = 0;
    for(i = 1;temp && temp->pid != pid; temp = temp->next, i++); // traverse to relevant pid

    if (i != 0)
    {
        if (!errOccured)
        {
            fprintf(stderr, "\r%s with pid = %d exited normally\n", temp->name, temp->pid);
            printf("\r%s ", prefix);
        }
        else
        {
            fprintf(stderr, "\r%s with pid = %d exited with exit code %d\n", temp->name, temp->pid, errOccured);
            printf("\r%s ", prefix);
        }
        fflush(stdout);
        fflush(stderr);
        head = removePid(head, pid);
    }

    return head;
}

struct bgLL *makeBgProcess(struct bgLL *head, char **argv)
{
    pid_t Pid = fork();
    if (Pid == -1) // fork error
    {
        perror(RED"Could not fork"RESETCOL);
        return head;
    }
    else if (Pid == 0) // child
    {
        setpgid(0, 0);
        
        if (argv == NULL)
            return head;

        if (execvp(argv[0], argv) == -1)
        {
            perror(RED"Could not execute command "RESETCOL);
            exit(EXIT_FAILURE);
            return head;
        }
    }
    else // parent
    {
        tcsetpgrp(Pid, getpgid(0));
        char cmnd[1000] = "";
        for (int i = 0; argv[i] != NULL; i++)
        {
            strcat(cmnd, " ");
            strcat(cmnd, argv[i]);
        }

        head = insertPid(head, Pid, cmnd, 'r');
        printf("[%d]\t%d\n", bgPcount, Pid);
    }

    return head;
}