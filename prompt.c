#include "prompt.h"

char *getPrefix(char *prefix, int update)
{
    struct utsname utStruct;
    if (update & 1)
    {
        uid_t ueuidUser = geteuid();
        struct passwd *pwdUser = getpwuid(ueuidUser);
        username = pwdUser->pw_name;

        if (username == NULL)
        {
            perror(RED"Could not resolve username"RESETCOL);
        }
    }
    if (update & 2)
    {
        if (uname(&utStruct) == -1)
        {
            perror(RED"Could not resolve systemname"RESETCOL);
            exit(EXIT_FAILURE);
        }

        sysname = utStruct.sysname;
        if (sysname == NULL)
        {
            perror(RED"Could not resolve sysname"RESETCOL);
            exit(EXIT_FAILURE);
        }
    }
    if (update & 4)
    {
        free(currdir);
        currdir = getCurrentDirectory();

        if (strncmp(currdir, pathHome, strlen(pathHome)) == 0)
        {
            sprintf(prefix, "\r\033[1;32m<%s@%s:~%s>\033[0m", username, sysname, currdir + strlen(pathHome));
            return prefix;
        }
        // ! where to check for null currdir? should exit??
    }

    sprintf(prefix, "\r\033[1;32m<%s@%s:%s>\033[0m", username, sysname, currdir);
    return prefix;
}