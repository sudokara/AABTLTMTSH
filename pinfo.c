#include "pinfo.h"

void printPinfoCurrent()
{
    pid_t currPid = getpid();
    printPinfoPid(currPid);    
}

void printPinfoPid(pid_t pid)
{
    char *pathToProcFile = malloc((PATH_MAX + 1) * sizeof(char));
    if (pathToProcFile == NULL)
    {
        perror(RED"Could not make space to store path"RESETCOL);
        return;
    }

    sprintf(pathToProcFile, "/proc/%d/stat", pid);
    FILE *procStat = fopen(pathToProcFile,"r");

    if (procStat == NULL)
    {
        perror(RED"Process may not exist, could not open stat file for process"RESETCOL);
        return;
    }

    // for pid
    printf("pid : %d\n", pid); 


    // for status {R/S/S+/Z}
    int pgrp,tpgid, vmem;
    char state;
    fscanf(procStat, "%*s %*s %c %*s %d %*s %*s %d %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d", &state, &pgrp, &tpgid, &vmem);

    char *status = malloc(3 * sizeof(char));
    if (status == NULL)
    {
        perror(RED"Could not make space to store status"RESETCOL);
        return;
    }
    if (pgrp == tpgid)
        sprintf(status, "%c+", state);
    else
        sprintf(status, "%c", state);

    fclose(procStat);
    if (status)
    {
        printf("Process status : %s\n", status);
        free(status);
    }
    else
    {
        printf("Error fetching process status\n");
    }

    // for virtual memory
    printf("memory : %d\n", vmem);

    // for executable path
    char procPath[64];
    sprintf(procPath, "/proc/%d/exe", pid);

    ssize_t writtenBytes = readlink(procPath, pathToProcFile, PATH_MAX); // read symlink
    if (writtenBytes == -1)
    {
        perror(RED"Could not read path of executable"RESETCOL);
        free(pathToProcFile);
        return;
    }

    if (writtenBytes != PATH_MAX)
    {
        pathToProcFile[writtenBytes] = '\0';
        if (strncmp(pathToProcFile, pathHome, strlen(pathHome)) == 0)
        {
            sprintf(pathToProcFile, "~%s",pathToProcFile + strlen(pathHome));
        }
        printf("executable path : %s\n", pathToProcFile);
    }

    free(pathToProcFile);
}