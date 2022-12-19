#include "cd.h"

char *getAbsPath(char *path)
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

char *changeDirect(char *path)
{
    if (chdir(path) == -1)
    {
        perror(RED"Could not change directory"RESETCOL);
        return NULL;
    }
    
    return path;
}

char *changeDirectoryUpdatePrefix(char *prefix, char *path)
{
    bool printCurrdir = false;
    char *path2;

    if (path == NULL || path[0] == '\0') // no path provided, open current directory
        path = pathHome;
    else if (!strcmp(path, "-")) // cd -
    {
        path = previousCd;
        printCurrdir = true;
    }
    else if (!strcmp(path, "~")) // cd ~
    {
        if (changeDirect(pathHome) == NULL)
            return NULL;

        getPrefix(prefix, 7);

        return prefix;
    }
    else if (path[0] == '~' && path[1]) // relative path
    {
        path2 = getAbsPath(path);
        if (changeDirect(path2) == NULL)
        {
            if (path2)
                free(path2);
            return NULL;
        }

        getPrefix(prefix, 7);

        free(path2);
        return prefix;
    }

    
    if (changeDirect(path) == NULL) // chdir failed
        return NULL;

    getPrefix(prefix, 7); // update prefix

    if (printCurrdir) // if cd -
        printCurrentDirectory();

    return prefix;
}