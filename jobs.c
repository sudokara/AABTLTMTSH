#include "bg.h"
#include "jobs.h"
#define MAX_BG_PROCESSES 128
/* Job *jobList = NULL;
int jobCount = 0;
int completedJob = 0;

void jobs(Job *jobList, int running, int stopped)
{
    if (jobList == NULL)
    {
        printf("No running jobs\n");
        return;
    }
    else
    {
        for (Job *temp = jobList; temp != NULL; temp = temp->next)
        {
            if ((temp->state == 'r' && running) || (temp->state == 's' && stopped))
            {
                printf("[%d] %s %s [%d]\n", temp->jobNum, temp->state == 'r' ? "Running" : "Stopped", temp->cmnd, temp->pid);
            }
        }
    }
}

void sig(Job *jobList, int signum, int jobNum)
{
    pid_t targetPid = 0;
    jobNum += completedJob;

    for (Job *temp = jobList; temp != NULL; temp = temp->next)
    {
        if (temp->jobNum == jobNum && temp->state == 'r')
        {
            targetPid = temp->pid;
            break;
        }
    }

    if (targetPid == 0)
    {
        fprintf(stderr, "Job number %d not found or is already stopped\n", jobNum);
        return;
    }

    if (kill(targetPid, signum) < 0)
        perror("Failed to send signal to process");
}

void fg(Job *jobList, int jobNum)
{
    pid_t targetPid = 0;
    Job *temp = NULL;
    jobNum += completedJob;

    for (temp = jobList; temp != NULL; temp = temp->next)
    {
        if (temp->jobNum == jobNum)
        {
            targetPid = temp->pid;
            break;
        }
    }

    if (targetPid == 0)
    {
        fprintf(stderr, "Job number %d not found\n", jobNum);
        return;
    }

    if (kill(targetPid, SIGCONT) < 0)
    {
        perror("Failed to continue process");
        return;
    }
    temp->state = 'r';
    int status;
    waitpid(targetPid, &status, 0);
}

void bg(Job *jobList, int jobNum)
{
    pid_t targetPid = 0;
    Job *temp = NULL;
    jobNum += completedJob;

    for (temp = jobList; temp != NULL; temp = temp->next)
    {
        if (temp->jobNum == jobNum)
        {
            targetPid = temp->pid;
            break;
        }
    }

    if (targetPid == 0)
    {
        fprintf(stderr, "Job number %d not found\n", jobNum);
        return;
    }

    if (kill(targetPid, SIGCONT) < 0)
    {
        perror("Failed to continue process");
        return;
    }
    temp->state = 'r';
} */

int cmp(const void *a, const void *b)
{
    struct bgLL *x = a;
    struct bgLL *y = b;

    return (strcmp(x->name, y->name));
}

void jobs(struct bgLL *head, int running, int stopped)
{
    struct bgLL *arr[MAX_BG_PROCESSES];
    int i = 0;
    if (head == NULL)
    {
        printf("No running jobs\n");
        return;
    }
    else
    {
        for (struct bgLL *temp = head; temp != NULL; temp = temp->next)
        {
            if ((temp->status == 'r' && running) || (temp->status == 's' && stopped))
            {
                // printf("[%d] %s %s [%d]\n", temp->jobNum, temp->status == 'r' ? "Running" : "Stopped", temp->name, temp->pid);
                arr[i++] = temp;
            }
        }
    }

    qsort(arr, i - 1, sizeof(struct bgLL *), cmp);
    for (int j = i - 1; j >= 0; j--)
    {
        printf("[%d] %s %s [%d]\n", arr[j]->jobNum, arr[j]->status == 'r' ? "Running" : "Stopped", arr[j]->name, arr[j]->pid);
    }
}

void sig(struct bgLL *head, int signum, int jobNum)
{
    pid_t targetPid = 0;
    // if (jobNum < bgCompleted)
    //     jobNum += bgCompleted;

    for (struct bgLL *temp = head; temp != NULL; temp = temp->next)
    {
        if (temp->jobNum == jobNum && temp->status == 'r')
        {
            targetPid = temp->pid;
            break;
        }
    }

    if (targetPid == 0)
    {
        fprintf(stderr, "Job number %d not found or is already stopped\n", jobNum);
        return;
    }

    if (kill(targetPid, signum) < 0)
        perror("Failed to send signal to process");
}

struct bgLL *fgFn(struct bgLL *head, int jobNum)
{
    struct bgLL *temp = NULL;
    // if (jobNum > bgCompleted)
    //     jobNum += bgCompleted;
    pid_t targetPid = 0;

    for (temp = head; temp != NULL; temp = temp->next)
    {
        if (temp->jobNum == jobNum)
        {
            targetPid = temp->pid;
            break;
        }
    }

    if (targetPid == 0)
    {
        fprintf(stderr, "Job number %d not found\n", jobNum);
        return head;
    }

    if (kill(targetPid, SIGCONT) < 0)
    {
        perror("Failed to continue process");
        return head;
    }
    Pid = targetPid;
    temp->status = 'r';
    int status;
    waitpid(targetPid, &status, 0);
    head = removePid(head, targetPid);
    return head;
}

void bgFn(struct bgLL *head, int jobNum)
{
    pid_t targetPid = 0;
    struct bgLL *temp = NULL;
    // if (jobNum > bgCompleted)
    //     jobNum += bgCompleted;

    for (temp = head; temp != NULL; temp = temp->next)
    {
        if (temp->jobNum == jobNum)
        {
            targetPid = temp->pid;
            break;
        }
    }

    if (targetPid == 0)
    {
        fprintf(stderr, "Job number %d not found\n", jobNum);
        return;
    }

    if (kill(targetPid, SIGCONT) < 0)
    {
        perror("Failed to continue process");
        return;
    }
    temp->status = 'r';
    int status;
    waitpid(targetPid, &status, WNOHANG);
}