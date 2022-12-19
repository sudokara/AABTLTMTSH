#include "input.h"
#define MAX_PIPE_ARGS 128

char *getAbsPathRedir(char *path)
{
    char *path2;

    if (path[0] == '~')
    {
        path2 = malloc(3 + PATH_MAX);
        strcpy(path2, pathHome);
        strcat(path2, path + 1);
    }
    else
        return path;

    return path2;
}

int handleRedir(char *cmnd)
{
    int len = strlen(cmnd);
    int outRedir = strcspn(cmnd, ">"), inRedir = strcspn(cmnd, "<");
    int appRedir = 0;
    char *tokens, *saveptr;

    if (outRedir != len || inRedir != len)
    {
        if (inRedir != len && outRedir != len) // both input and output redirection
        {
            // cmnd[inRedir] = 0;
            // char *inputRight = cmnd + inRedir + 1;
            // cmnd[outRedir] = 0;
            // char *outputRight = cmnd + outRedir + 1;
            // printf("boht : |%s| |%s| |%s|\n", cmnd, inputRight, outputRight);

            // if (inRedir < outRedir) // first input redirection
            // {
                // printf("the order is |%s| |%s| |%s|\n", inputRight, cmnd, outputRight);

                if (outRedir < inRedir)
                    goto onlyOut;

                cmnd[inRedir] = 0;
                char *inputRight = cmnd + inRedir + 1;
                inputRight = strtok(inputRight, " \t\n");
                int fd1 = open(inputRight, O_RDONLY);
                if (fd1 < 0)
                {
                    fprintf(stderr, RED "Could not open file for redirection" RESETCOL);
                    return 2;
                }
                char *outputRight;
                if (cmnd[outRedir + 1] && cmnd[outRedir + 1] == '>')
                {
                    appRedir = 1;
                    cmnd[outRedir] = cmnd[outRedir + 1] = 0;
                    outputRight = cmnd + outRedir + 2;
                }

                if (!appRedir)
                {
                    cmnd[outRedir] = 0;
                    outputRight = cmnd + outRedir + 1;
                }
                outputRight = strtok(outputRight, " \t\n");

                int fd2;
                if (!appRedir)
                    fd2 = open(outputRight, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                else
                    fd2 = open(outputRight, O_APPEND | O_CREAT | O_WRONLY, 0644);

                if (fd2 < 0)
                {
                    fprintf(stderr, RED "Could not open file for redirection" RESETCOL);
                    return 2;
                }

                int stdinCopy = dup(STDIN_FILENO), stdoutCopy = dup(STDOUT_FILENO);
                dup2(fd1, STDIN_FILENO);
                dup2(fd2, STDOUT_FILENO);
                if (handleInput(cmnd, 1) == 1)
                {
                    close(fd1);
                    close(fd2);
                    dup2(stdinCopy, STDIN_FILENO);
                    dup2(stdoutCopy, STDOUT_FILENO);
                    return 1;
                }
                close(fd1);
                close(fd2);
                dup2(stdinCopy, STDIN_FILENO);
                dup2(stdoutCopy, STDOUT_FILENO);
                return 0;
            // }
            // else // first output redirection
            // {
            //     printf("the order is |%s| |%s| |%s|\n", cmnd, inputRight, outputRight);
            // }

            return 0;
        }
        else if (inRedir != len) // only input redirection
        {
            cmnd[inRedir] = 0;
            char *inputRight = cmnd + inRedir + 1;
            inputRight = strtok(inputRight, " \t\n");
            int fd = open(inputRight, O_RDONLY);

            if (fd < 0)
            {
                fprintf(stderr, RED "Could not open file for redirection" RESETCOL);
                return 2;
            }

            int stdinCopy = dup(STDIN_FILENO);
            dup2(fd, STDIN_FILENO);
            if (handleInput(cmnd, 1) == 1)
            {
                close(fd);
                dup2(stdinCopy, STDIN_FILENO);
                return 1;
            }
            close(fd);
            dup2(stdinCopy, STDIN_FILENO);
            return 0;
        }
        else // only output redirection
        {
            onlyOut:
            printf("");
            char *outputRight;
            if (cmnd[outRedir + 1] && cmnd[outRedir + 1] == '>')
            {
                appRedir = 1;
                cmnd[outRedir] = cmnd[outRedir + 1] = 0;
                outputRight = cmnd + outRedir + 2;
            }

            if (!appRedir)
            {
                cmnd[outRedir] = 0;
                outputRight = cmnd + outRedir + 1;
            }
            outputRight = strtok(outputRight, " \t\n");

            // printf("|%s| |%s| %d\n", cmnd, outputRight, appRedir);

            int fd;
            if (!appRedir)
                fd = open(outputRight, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            else
                fd = open(outputRight, O_APPEND | O_CREAT | O_WRONLY, 0644);

            if (fd < 0)
            {
                fprintf(stderr, RED "Could not open file for redirection" RESETCOL);
                return 2;
            }

            // printf("only out |%s| |%s|\n", cmnd, outputRight);
            int stdoutCopy = dup(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            if (handleInput(cmnd, 1) == 1)
            {
                close(fd);
                dup2(stdoutCopy, STDOUT_FILENO);
                return 1;
            }
            close(fd);
            dup2(stdoutCopy, STDOUT_FILENO);
            return 0;
        }
    }
    /////////////////////////////
    // printf("no redir\n");
    ////////////////////////////
    if (handleInput(cmnd, 1) == 1)
        return 1;

    return 0;
}

int handlePipe(char *cmnd)
{
    if (strcspn(cmnd, "|") == strlen(cmnd))
    {
        // if (handleInput(cmnd, 1) == 1)
        if (handleRedir(cmnd) == 1)
            exit(EXIT_SUCCESS);
        return 0;
    }
    char *tokens, *saveptr;
    tokens = strtok_r(cmnd, "|", &saveptr);
    int stdinCopy = STDIN_FILENO, stdoutCopy = STDOUT_FILENO;
    int fd[2];                                                            // for pipe
    int inFd = dup(STDIN_FILENO), outFd = dup(STDOUT_FILENO), status = 0; // first process input is from stdin
    pid_t pid;

    char *argv[MAX_PIPE_ARGS];
    int argc = 0;
    while (tokens)
    {
        argv[argc++] = tokens;
        tokens = strtok_r(NULL, "|", &saveptr);
    }

    for (int i = 0; i < argc; i++)
    {
        if (pipe(fd) < 0)
        {
            perror("pipe");
            return 2;
        }

        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return 2;
        }
        else if (pid == 0)
        {
            close(fd[0]);
            if (i == argc - 1)
            {
                dup2(outFd, STDOUT_FILENO);
                close(outFd);
            }
            else
            {
                dup2(fd[1], STDOUT_FILENO);
            }

            // handleInput(argv[i], 1);
            handleRedir(argv[i]);
            close(fd[1]);
            dup2(inFd, STDIN_FILENO);
            close(inFd);
            exit(EXIT_SUCCESS);
        }
        else
        {
            waitpid(pid, &status, WUNTRACED);
            close(fd[1]);
            if (i == argc - 1)
            {
                dup2(inFd, STDIN_FILENO);
                close(inFd);
            }
            else
            {
                dup2(fd[0], STDIN_FILENO);
            }
        }
    }

    return 0;
}