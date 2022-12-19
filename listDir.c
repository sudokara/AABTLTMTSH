#include "listDir.h"

char *getAbsPathLS(char *path)
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

void listDir(int allDisplay, int argc, char *argv[], int idx)
{
    struct dirent **nameList;
    struct stat sb;
    int n = -1;
    char path[PATH_MAX + 20];

    if (argc == 0) // no target directory given, open current directory
    {
        n = scandir(".", &nameList, NULL, alphasort);

        if (n < 0)
        {
            perror(RED "Could not scan directory" RESETCOL);
            return;
        }

        int i = 0;
        while (i < n)
        {
            strcpy(path, "./");
            strcat(path, nameList[i]->d_name);
            if (nameList[i]->d_name[0] == '.' && !allDisplay)
            {
                free(nameList[i]);
                i++;
                continue;
            }
            lstat(path, &sb);

            if (S_ISDIR(sb.st_mode))
                printf(BLUE);
            else
            {
                if (sb.st_mode & S_IXUSR)
                    printf(GREEN);
                else
                    ;
            }

            printf("%s\t" RESETCOL, nameList[i]->d_name);
            free(nameList[i]);
            i++;
        }
        free(nameList);
        printf("\n");
    }
    else // one or more target directories given
    {
        for (int i = 0; i < argc; i++) // go through each target directory
        {
            printf("\n");
            strcpy(path, argv[idx + i]);
            char *abspath = getAbsPathLS(path);
            strcpy(path, abspath);

            lstat(path, &sb);
            if (S_ISREG(sb.st_mode))
            {
                if (sb.st_mode & S_IXUSR)
                    printf(GREEN);

                printf("%s\n" RESETCOL, path);
                continue;
            }

            n = scandir(path, &nameList, NULL, alphasort);
            if (n < 0)
            {
                perror(RED "Could not scan directory" RESETCOL);
                return;
            }

            char filePath[512];

            printf("%s: \n", argv[idx + i]);

            int i = 0;
            while (i < n) // go through every item in the directory
            {
                strcpy(filePath, path);

                if (filePath[strlen(filePath) - 1] != '/')
                    strcat(filePath, "/");

                strcat(filePath, nameList[i]->d_name);
                lstat(filePath, &sb);
                if (nameList[i]->d_name[0] == '.' && !allDisplay)
                {
                    free(nameList[i]);
                    i++;
                    continue;
                }

                if (S_ISDIR(sb.st_mode))
                    printf(BLUE);
                else
                {
                    if (sb.st_mode & S_IXUSR)
                        printf(GREEN);
                    else
                        ;
                }

                printf("%s\t" RESETCOL, nameList[i]->d_name);

                free(nameList[i]);
                i++;
            }
            free(nameList);
            printf("\n");
        }
    }
}

void listLongDir(int allDisplay, int argc, char *argv[], int idx)
{
    struct dirent **nameList;
    struct stat sb;
    int n = -1;
    char path[PATH_MAX + 20];

    if (argc == 0) // no target dir
    {
        n = scandir(".", &nameList, NULL, alphasort);

        if (n < 0)
        {
            perror("could not scan directory");
            return;
        }

        int i = 0;

        // finding total blocks
        long totalBlocks = 0;
        while (i < n)
        {
            strcpy(path, "./");
            strcat(path, nameList[i]->d_name);
            if (nameList[i]->d_name[0] == '.')
            {
                i++;
                continue;
            }
            lstat(path, &sb);
            totalBlocks += sb.st_blocks;
            i++;
        }

        totalBlocks /= 2;

        printf("Total %ld\n", totalBlocks);

        i = 0;

        while (i < n)
        {
            strcpy(path, "./");
            strcat(path, nameList[i]->d_name);
            if (nameList[i]->d_name[0] == '.' && !allDisplay)
            {
                free(nameList[i]);
                i++;
                continue;
            }
            lstat(path, &sb);

            printf("%s", S_ISDIR(sb.st_mode) ? "d" : "-");
            fflush(stdout);
            char *permissions[] = {"r", "w", "x"};
            int permMacros[] = {S_IRUSR, S_IWUSR, S_IXUSR};

            for (int i = 0; i <= 6; i += 3)
            {
                for (int j = 0; j < 3; j++)
                {
                    char toWrite[100] = "";
                    sprintf(toWrite, "%s", ((sb.st_mode & (permMacros[j] >> i)) > 0) ? permissions[j] : "-");
                    write(1, toWrite, sizeof(toWrite));
                }
            }

            fflush(stdout);
            char date[100];
            strftime(date, 99, "%b %-d %H:%M", localtime(&(sb.st_mtime)));
            struct passwd *pwdUser = getpwuid(sb.st_uid);
            struct group *pwdGroup = getgrgid(sb.st_gid);
            printf("\t%ld\t%s\t%s\t%ld\t%s\t", sb.st_nlink, pwdUser->pw_name, pwdGroup->gr_name, sb.st_size, date);
            fflush(stdout);

            if (S_ISDIR(sb.st_mode))
                printf(BLUE);
            else
            {
                if (sb.st_mode & S_IXUSR)
                    printf(GREEN);
                else
                    ;
            }

            printf("%s\n" RESETCOL, nameList[i]->d_name);
            free(nameList[i]);
            i++;
        }
        free(nameList);
        printf("\n");
    }
    else
    {
        for (int i = 0; i < argc; i++) // open every target dir
        {
            printf("\n");
            strcpy(path, argv[idx + i]);
            char *abspath = getAbsPathLS(path);
            strcpy(path, abspath);

            lstat(path, &sb);
            if (S_ISREG(sb.st_mode))
            {
                if (sb.st_mode & S_IXUSR)
                    printf(GREEN);

                printf("%s\n" RESETCOL, path);
                continue;
            }

            n = scandir(path, &nameList, NULL, alphasort);
            if (n < 0)
            {
                perror(RED "Could not scan directory" RESETCOL);
                return;
            }

            char filePath[512];

            printf("%s: \n", argv[idx + i]);

            int i = 0;

            // finding total blocks
            long totalBlocks = 0;
            while (i < n)
            {
                strcpy(filePath, path);

                if (filePath[strlen(filePath) - 1] != '/')
                    strcat(filePath, "/");

                strcat(filePath, nameList[i]->d_name);
                lstat(filePath, &sb);
                if (nameList[i]->d_name[0] == '.')
                {
                    i++;
                    continue;
                }
                lstat(path, &sb);
                totalBlocks += sb.st_blocks;
                i++;
            }

            totalBlocks /= 2;

            printf("Total %ld\n", totalBlocks);

            i = 0;

            while (i < n) // handle every item in directory
            {
                strcpy(filePath, path);

                if (filePath[strlen(filePath) - 1] != '/')
                    strcat(filePath, "/");

                strcat(filePath, nameList[i]->d_name);
                lstat(filePath, &sb);
                if (nameList[i]->d_name[0] == '.' && !allDisplay)
                {
                    free(nameList[i]);
                    i++;
                    continue;
                }
                lstat(filePath, &sb);

                printf("%s", S_ISDIR(sb.st_mode) ? "d" : "-");
                fflush(stdout);
                char *permissions[] = {"r", "w", "x"};
                int permMacros[] = {S_IRUSR, S_IWUSR, S_IXUSR};

                for (int i = 0; i <= 6; i += 3)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        char toWrite[100] = "";
                        sprintf(toWrite, "%s", ((sb.st_mode & (permMacros[j] >> i)) > 0) ? permissions[j] : "-");
                        write(1, toWrite, sizeof(toWrite));
                    }
                }

                fflush(stdout);
                char date[100];
                strftime(date, 99, "%b %-d %H:%M", localtime(&(sb.st_mtime)));
                struct passwd *pwdUser = getpwuid(sb.st_uid);
                struct group *pwdGroup = getgrgid(sb.st_gid);
                printf("\t%ld\t%s\t%s\t%ld\t%s\t", sb.st_nlink, pwdUser->pw_name, pwdGroup->gr_name, sb.st_size, date);
                fflush(stdout);

                if (S_ISDIR(sb.st_mode))
                    printf(BLUE);
                else
                {
                    if (sb.st_mode & S_IXUSR)
                        printf(GREEN);
                    else
                        ;
                }

                printf("%s\n" RESETCOL, nameList[i]->d_name);
                free(nameList[i]);
                i++;
            }
            free(nameList);
            printf("\n");
        }
    }
}

void listDirCaller(int allDisplay, int longDisplay, int argc, char *argv[], int idx)
{
    if (!longDisplay)
    {
        listDir(allDisplay, argc, argv, idx);
    }
    else
    {
        listLongDir(allDisplay, argc, argv, idx);
    }
}