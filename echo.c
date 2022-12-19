#include "echo.h"

void echo(char *msg)
{
    char *saveptr;
    msg = strtok_r(msg, " \t", &saveptr);
    while (msg)
    {
        printf("%s ", msg);
        msg = strtok_r(NULL, " \t", &saveptr);
    }
    printf("\n");
}