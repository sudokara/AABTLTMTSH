#include "discover.h"

char *getAbsPathD(char *path)
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

void discoverAll(char *path, int files, int directories)
{
    if (path)
    {
        struct stat sb;
        DIR *dir = opendir(path); // open given directory
        if (!dir)
            return;

        struct dirent *dirEntry;
        while ((dirEntry = readdir(dir)))
        {
            char fpath[strlen(path) + 2 + strlen(dirEntry->d_name)];
            char printpath[strlen(path) + 2 + strlen(dirEntry->d_name)];
            strcpy(fpath, path);
            if (path[strlen(path) - 1] != '/')
                strcat(fpath, "/");
            if (strcmp(dirEntry->d_name, ".") && strcmp(dirEntry->d_name, "..")) // if not . and ..
            {
                strcat(fpath, dirEntry->d_name);
                lstat(fpath, &sb);
                if (S_ISDIR(sb.st_mode))
                {
                    discoverAll(fpath, files, directories);
                    if (directories)
                    {
                        if (strncmp(fpath, pathHome, strlen(pathHome)) == 0)
                        {
                            sprintf(printpath, "~%s", fpath + strlen(pathHome));
                            printf(BLUE "%s\n" RESETCOL, printpath);
                        }
                        else
                            printf(BLUE "%s\n" RESETCOL, fpath);
                    }
                }
                else
                {
                    if (files)
                    {
                        if (strncmp(fpath, pathHome, strlen(pathHome)) == 0)
                        {
                            sprintf(printpath, "~%s", fpath + strlen(pathHome));

                            if (sb.st_mode & S_IXUSR)
                                printf(GREEN "%s\n" RESETCOL, printpath);
                            else
                                printf("%s\n", printpath);
                        }
                        else
                        {
                            if (sb.st_mode & S_IXUSR)
                                printf(GREEN "%s\n" RESETCOL, fpath);
                            else
                                printf("%s\n", fpath);
                        }
                    }
                }
            }
        }

        closedir(dir);
    }
}

void discoverThing(char *path, char *target, int files, int directories)
{
    if (path)
    {
        struct stat sb;
        DIR *dir = opendir(path);
        if (!dir)
            return;

        struct dirent *dirEntry;
        while ((dirEntry = readdir(dir)))
        {
            char fpath[strlen(path) + 2 + strlen(dirEntry->d_name)];
            strcpy(fpath, path);
            if (path[strlen(path) - 1] != '/')
                strcat(fpath, "/");
            if (strcmp(dirEntry->d_name, ".") && strcmp(dirEntry->d_name, ".."))
            {
                strcat(fpath, dirEntry->d_name);
                lstat(fpath, &sb);
                if (S_ISDIR(sb.st_mode))
                {
                    discoverThing(fpath, target, files, directories);
                    if (directories && !strcmp(target, dirEntry->d_name))
                        printf(BLUE "%s\n" RESETCOL, fpath);
                }
                else
                {
                    if (files && !strcmp(target, dirEntry->d_name))
                    {
                        if (sb.st_mode & S_IXUSR)
                            printf(GREEN "%s\n" RESETCOL, fpath);
                        else
                            printf("%s\n", fpath);
                    }
                }
            }
        }

        closedir(dir);
    }
}

void discover(int argc, char *argv[], int index, int files, int directories)
{
    if (argc == 0) // only discover given
    {
        discoverAll(".", files, directories);
    }
    else
    {
        if (argc == 1) // discover with target directory or file
        {
            if (argv[index][0] == '"') 
            {
                // only file to find has been given
                char *target = argv[index] + 1;
                if (target[strlen(target) - 1] == '"')
                    target[strlen(target) - 1] = '\0';
                else
                {
                    printf(RED "Enclose file to search for in quotes\n" RESETCOL);
                    return;
                }
                discoverThing(".", target, files, directories);
            }
            else
            {
                // path to use has been given
                discoverAll(getAbsPathD(argv[index]), files, directories);
            }
        }
        else if (argc == 2) // both target directory and file
        {
            if (argv[index][0] == '"')
            {
                // file
                char *target = argv[index] + 1;
                if (target[strlen(target) - 1] == '"')
                    target[strlen(target) - 1] = '\0';
                else
                {
                    printf(RED "Enclose file to search for in quotes\n" RESETCOL);
                    return;
                }
                discoverThing(getAbsPathD(argv[index + 1]), target, files, directories);
            }
            else
            {
                // target directory
                char *target = argv[index + 1] + 1;
                if (target[strlen(target) - 1] == '"')
                    target[strlen(target) - 1] = '\0';
                else
                {
                    printf(RED "Enclose file to search for in quotes\n" RESETCOL);
                    return;
                }
                discoverThing(getAbsPathD(argv[index]), target, files, directories);
            }
        }
    }
    return;
}