#include "pwd.h"

char *getCurrentDirectory()
{
    char *cwd = malloc((PATH_MAX + 4) * sizeof(char));

    if (cwd == NULL || getcwd(cwd, PATH_MAX) == NULL)
    {
        perror(RED"Could not resolve current working directory"RESETCOL);
        return NULL;
    }

    return cwd;
}

void printCurrentDirectory()
{
    char *pwd = getCurrentDirectory();
    if (pwd == NULL)
        return;
    printf("%s\n", pwd);
    free(pwd);
}