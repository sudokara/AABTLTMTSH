#include "fg.h"

long long createForegroundProcess(char **argv)
{
    waitFg = 1;
    time_t begin = time(NULL); // for calculating time taken
    Pid = fork();
    if (Pid == -1) // fork error
    {
        perror(RED "Could not fork" RESETCOL);
        return -1;
    }
    else if (Pid == 0) // child
    {
        if (argv == NULL)
        {
            fprintf(stderr, RED"Expected arguments\n"RESETCOL);
            exit(EXIT_FAILURE);
        }
        tcsetpgrp(0, 0);
        // setpgid(0, 0);
        if (execvp(argv[0], argv) == -1)
        {
            perror(RED "Could not execute command " RESETCOL);
            waitFg = 0;
            // return -1;
            exit(EXIT_FAILURE);
        }
    }
    else // parent
    {
        int status, wpid;
        while (waitFg != 0 && (wpid = waitpid(Pid, &status, WUNTRACED)) > 0);
        time_t end = time(NULL);
        waitFg = 1;
        return end - begin;
    }

    return -1;
}