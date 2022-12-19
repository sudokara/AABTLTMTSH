#include "headers.h"

char *username, *sysname, *currdir, *prefix, *pathHome, *previousCd, *prepreviousCd;
struct historyStack *history;
long long timeTaken = -1;
struct bgLL *head = NULL;
pid_t mainPid;
char cmndCopy[PATH_MAX + 20];

struct termios orig_termios;
void die(const char *s)
{
    perror(s);
    exit(1);
}

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

void sigchld_handler(int signum)
{
    pid_t pid;
    int errnocopy = errno;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // ! handle pid
        if (status == 0)
            head = handlePid(head, pid, 0);
        else
            head = handlePid(head, pid, status);
    }
    errno = errnocopy;
}

void sigint_handler(int signum)
{
    if (Pid > 0)
    {
        kill(Pid, SIGINT);
        Pid = -1;
    }
    printf("\n%s ", prefix);
    fflush(stdout);
}

void sigtstp_handler(int signum)
{
    pid_t pid = getpid();
    if (pid == mainPid)
    {
        if (Pid > 0)
        {
            waitFg = 0;
            head = insertPid(head, Pid, cmndCopy, 's');
            setpgid(Pid, Pid);
            kill(Pid, SIGTSTP);
        }
        else
        {
            printf("\n%s ", prefix);
            fflush(stdout);
        }
    }
}

void clearScreen()
{
    printf("\033[H\033[J");
}

void checkTime()
{
    if (timeTaken != -1)
    {
        if (timeTaken)
            printf(GREEN "\b\btook %llds> " RESETCOL, timeTaken);
        timeTaken = -1;
    }
}

int handleInput(char *inp, int fg)
{
    /////////////////////////////////
    // printf("|%s|\n", inp);
    ////////////////////////////////
    char *saveptr = NULL;
    char *tokens = __strtok_r(inp, " \t\n", &saveptr);

    if (tokens)
    {
        if (!strcmp(tokens, "exit"))
        {
            printf("Exiting...\n");
            free(prefix);
            destroyAllPid(head);
            storeHistory(history, "./.shhistory");
            return 1;
        }
        else if (!strcmp(tokens, "pwd"))
        {
            // ! pwd
            printCurrentDirectory();
        }
        else if (!strcmp(tokens, "cd"))
        {
            // !cd
            char *path = __strtok_r(NULL, " \t\n", &saveptr);
            tokens = __strtok_r(NULL, " \t\n", &saveptr);

            if (tokens != NULL)
            {
                printf(RED "Too many arguments for cd\n" RESETCOL);
            }
            else
            {
                if ((path && strcmp(path, "-")) || (!path))
                {
                    previousCd = getCurrentDirectory();
                }
                else
                    prepreviousCd = getCurrentDirectory();

                changeDirectoryUpdatePrefix(prefix, path);

                if (path && !strcmp(path, "-"))
                    previousCd = prepreviousCd;
            }
        }
        else if (!strcmp(tokens, "echo"))
        {
            tokens = __strtok_r(NULL, " \t\n", &saveptr);
            while (tokens)
            {
                echo(tokens);
                tokens = __strtok_r(NULL, " \t\n", &saveptr);
            }
        }
        else if (strcmp(tokens, "ls") == 0 || strcmp(tokens, "ls") == ' ')
        {
            int argc = 0, longdisp = 0, alldisp = 0, gopt = -1;
            char *argv[64];
            while (tokens && argc + 1 != 63)
            {
                if (strcspn(tokens, ";") != strlen(tokens))
                    break;
                // printf("|%s|\n", tokens);
                argv[argc++] = tokens;
                tokens = __strtok_r(NULL, " \t\n", &saveptr);
            }
            argv[argc] = NULL;

            optind = 0;
            opterr = 0;

            while ((gopt = getopt(argc, argv, "al")) != -1)
            {
                switch (gopt)
                {
                case 'a':
                    alldisp = 1;
                    break;
                case 'l':
                    longdisp = 1;
                    break;
                default:
                    printf(RED "Invalid flag for ls\n" RESETCOL);
                    return 0;
                }
            }

            listDirCaller(alldisp, longdisp, argc - optind, argv, optind);
        }
        else if (!strcmp(tokens, "discover"))
        {
            int argc = 0, directories = 0, files = 0, gopt = -1;
            char *argv[64];
            while (tokens && argc + 1 != 63)
            {
                if (strcspn(tokens, ";") != strlen(tokens))
                    break;

                argv[argc++] = tokens;
                tokens = strtok_r(NULL, " \t\n", &saveptr);
            }
            argv[argc] = NULL;

            optind = 0;
            opterr = 0;

            while ((gopt = getopt(argc, argv, "fd")) != -1)
            {
                switch (gopt)
                {
                case 'f':
                    files = 1;
                    break;
                case 'd':
                    directories = 1;
                    break;
                default:
                    printf(RED "Invalid flag for discover\n" RESETCOL);
                    return 0;
                }
            }

            if (!files && !directories)
                files = directories = 1;

            discover(argc - optind, argv, optind, files, directories);
        }
        else if (!strcmp(tokens, "clear"))
        {
            clearScreen();
        }
        else if (!strcmp(tokens, "history"))
        {
            printHistory(history);
        }
        else if (!strcmp(tokens, "pinfo"))
        {
            tokens = __strtok_r(NULL, " \t\n", &saveptr);

            if (tokens == NULL)
            {
                //! current process
                printPinfoCurrent();
            }
            else
            {
                pid_t requestedPid = atoi(tokens);
                printPinfoPid(requestedPid);
            }
        }
        else if (!strcmp(tokens, "jobs"))
        {
            int argc = 0, running = 0, stopped = 0, gopt = -1;
            char *argv[64];
            while (tokens && argc + 1 != 63)
            {
                if (strcspn(tokens, ";") != strlen(tokens))
                    break;

                argv[argc++] = tokens;
                tokens = strtok_r(NULL, " \t\n", &saveptr);
            }
            argv[argc] = NULL;

            optind = 0;
            opterr = 0;

            while ((gopt = getopt(argc, argv, "rs")) != -1)
            {
                switch (gopt)
                {
                case 'r':
                    running = 1;
                    break;
                case 's':
                    stopped = 1;
                    break;
                default:
                    printf(RED "Invalid flag for jobs\n" RESETCOL);
                    return 0;
                }
            }

            if (!running && !stopped)
                running = stopped = 1;

            jobs(head, running, stopped);
        }
        else if (!strcmp(tokens, "sig"))
        {
            int jobnum, signum;
            tokens = strtok_r(NULL, " ", &saveptr);
            jobnum = atoi(tokens);
            tokens = strtok_r(NULL, " ", &saveptr);
            signum = atoi(tokens);
            sig(head, signum, jobnum);
        }
        else if (!strcmp(tokens, "fg"))
        {
            int jobnum;
            tokens = strtok_r(NULL, " ", &saveptr);
            if (tokens)
            {
                jobnum = atoi(tokens);
                head = fgFn(head, jobnum);
            }
            else
            {
                fprintf(stderr, RED "Expected job number\n" RESETCOL);
            }
        }
        else if (!strcmp(tokens, "bg"))
        {
            int jobnum;
            tokens = strtok_r(NULL, " ", &saveptr);
            if (tokens)
            {
                jobnum = atoi(tokens);
                bgFn(head, jobnum);
            }
            else
            {
                fprintf(stderr, RED "Expected job number\n" RESETCOL);
            }
        }
        else
        {
            char *argv[20] = {NULL};
            int argc = 0;

            while (tokens && argc + 1 != 19)
            {

                argv[argc] = tokens;
                argc++;
                tokens = __strtok_r(NULL, " \t\n", &saveptr);
            }
            argv[argc] = NULL;

            if (fg)
            {
                timeTaken = createForegroundProcess(argv);
                if (timeTaken == -1)
                    return 1;
            }
            else
                head = makeBgProcess(head, argv);
        }
    }
    else
    {
        ;
    }

    return 0;
}

void formatInput(char *cmnd) // checks which process is supposed to be a background process
{
    char *endptr = cmnd + strlen(cmnd) - 1;

    char *temp = cmnd, *prev = NULL, *saveptr, *tokens;
    int len = 0;
    if (strcspn(cmnd, "&") == strlen(cmnd))
    {
        // !only foreground, passs on to handleInput if not \n
        tokens = strtok(cmnd, ";\n");
        while (tokens)
        {
            if (tokens[0] != '\n')
                // if (handleInput(tokens, 1) == 1)
                if (handlePipe(tokens) == 1)
                    exit(EXIT_SUCCESS);
            tokens = strtok(NULL, ";\n");
        }
    }
    else
    {
        while (temp && *temp)
        {
            len = strcspn(temp, "&");
            if (len - 1 >= 0 && temp[len - 1] == ';')
            {
                printf(RED "Syntax error, unexpected ; or end of line\n" RESETCOL);
                return;
            }
            temp[len] = '\0';

            int len2 = strcspn(temp, ";");
            int validInp = 1;
            if (len2 != strlen(temp))
            {
                validInp = 0;
                for (int i = len2 + 1; temp[i] != '\0'; i++)
                {
                    if (temp[i] != ' ')
                    {
                        validInp = 1;
                        break;
                    }
                }
            }

            if (!validInp)
            {
                printf(RED "Syntax error, unexpected ; or end of line\n" RESETCOL);
                return;
            }

            if (temp[0] == '\0' || temp[0] == ';')
            {
                // ! syntax error
                printf(RED "Syntax error, unexpected ; or end of line\n" RESETCOL);
                return;
            }

            if (strlen(temp) - 1 >= 0 && temp[strlen(temp) - 1] != '\n')
            {
                // one of these is a background process
                tokens = strtok_r(temp, ";\n", &saveptr);
                while (tokens)
                {
                    prev = tokens;
                    tokens = strtok_r(NULL, ";\n", &saveptr);
                    if (tokens)
                    {
                        //! prev has a foreground process
                        if (handleInput(prev, 1) == 1)
                            // if (handlePipe(tokens) == 1)
                            exit(EXIT_SUCCESS);
                    }
                }
                if (tokens == NULL && prev != NULL && ((prev + strlen(prev) - 1) != endptr))
                {
                    // ! prev has a background process
                    if (handleInput(prev, 0) == 1)
                        exit(EXIT_SUCCESS);
                }
                else if (handleInput(prev, 1) == 1)
                    exit(EXIT_SUCCESS);
            }
            else
            {
                // ! temp has a foreground process
                // if (handleInput(temp, 1) == 1)
                if (handlePipe(temp) == 1)
                    exit(EXIT_SUCCESS);
            }
            temp += len + 1;
        }
    }
}

int main()
{
    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);

    mainPid = getpid();
    printf("\033[H\033[J"); // clear screen
    printf("AABTLTMTSH - Ambitious aspirations but too little time makes this shell hacky\n");

    currdir = malloc(PATH_MAX * sizeof(char));
    if (currdir == NULL)
    {
        perror(RED "Could not malloc for currdir" RESETCOL);
        exit(EXIT_FAILURE);
    }
    strcpy(currdir, "~");

    prepreviousCd = previousCd = pathHome = getCurrentDirectory();

    prefix = malloc((32 + 256 + PATH_MAX + 20) * sizeof(char));
    if (prefix == NULL)
    {
        perror(RED "Could not create prefix" RESETCOL);
        exit(EXIT_FAILURE);
    }

    history = makeHistoryStack(20);
    fetchHistory(history, "./.shhistory");

    getPrefix(prefix, 3);

    char inp[PATH_MAX + 20];

    while (true)
    {
        char c;
        struct dirent **namelist;
        int n = scandir(".", &namelist, NULL, alphasort);
        if (n == -1)
        {
            perror("scandir");
            exit(EXIT_FAILURE);
        }
        setbuf(stdout, NULL);
        memset(inp, 0, PATH_MAX + 20);
        enableRawMode();
        int pt = 0;
        printf("%s ", prefix);
        checkTime();
        fflush(stdout);
        char *possibles[20];
        int poss = 0;
        while (read(STDIN_FILENO, &c, 1) == 1)
        {
            char str[1000];
            if (iscntrl(c))
            {
                if (c == 10)
                    goto inputDone;
                else if (c == 27)
                {
                }
                else if (c == 127)
                { // backspace
                    if (pt > 0)
                    {
                        if (inp[pt - 1] == 9)
                        {
                            for (int i = 0; i < 7; i++)
                            {
                                printf("\b");
                            }
                        }
                        inp[--pt] = '\0';
                        printf("\b \b");
                    }
                }
                else if (c == 9)
                { // TAB character
                    char *inp2 = inp + strlen(inp) - 1;
                    while (inp2 > inp && *inp2 != ' ')
                        inp2--;

                    if (inp2 < inp + strlen(inp) - 1)
                        inp2++;
                    if (inp2)
                    {

                        strcpy(str, inp2);
                        // printf("\n|%s|\n", inp2);
                        int len = strlen(str);
                        int count = 0, idx = -1, matches = 0;
                        int matched[20];

                        for (int i = 0; i < n; i++)
                        {
                            if (!strncmp(str, namelist[i]->d_name, len))
                            {
                                int temp = 0;
                                for (int j = 0; namelist[i]->d_name[j] && str[j] && namelist[i]->d_name[j] == str[j]; j++)
                                    temp++;

                                if (temp > count)
                                {
                                    count = temp;
                                    idx = i;
                                }
                                if (temp > 0)
                                {
                                    if (matches <= 19)
                                        matched[matches] = i;
                                    matches++;
                                }
                            }
                        }
                        if (matches == 1)
                        {
                            printf("%s", namelist[idx]->d_name + len);
                            fflush(stdout);
                            pt += strlen(namelist[idx]->d_name + len);
                            strcat(inp, namelist[idx]->d_name + len);
                            struct stat sb;
                            lstat(namelist[idx]->d_name, &sb);
                            if (S_ISDIR(sb.st_mode))
                            {
                                pt += 1;
                                printf("/");
                                fflush(stdout);
                                strcat(inp, "/");
                            }
                        }
                        else if (matches > 1)
                        {
                            memset(possibles, 0, 20 * sizeof(char *));
                            poss = 0;
                            int *printed = calloc(matches, sizeof(int));
                            printf("\n");
                            for (int j = strlen(str);; j++)
                            {
                                char ch = namelist[matched[0]]->d_name[j];
                                for (int i = 0; i < matches; i++)
                                {
                                    if (namelist[matched[i]]->d_name[j] == '\0' || namelist[matched[i]]->d_name[j] != ch)
                                        goto done;
                                    if (!printed[i])
                                    {
                                        printf("%s\t", namelist[matched[i]]->d_name);
                                        printed[i] = 1;
                                    }
                                    possibles[poss] = strdup(namelist[matched[i]]->d_name);
                                    poss = (poss + 1 ) % 20;
                                }
                                fflush(stdout);
                                pt += 1;
                                char *str2 = malloc(2);
                                sprintf(str2, "%c", ch);
                                strcat(inp, str2);
                            }
                        done:
                            printf("\n");
                            printf("\n%s %s", prefix, inp);
                            fflush(stdout);
                            free(printed);
                        }
                        else
                            ;
                    }
                }
                else if (c == 4)
                {
                    exit(0);
                }
                else
                {
                }
            }
            else
            {
                inp[pt++] = c;
                printf("%c", c);
            }
        }

        printf("Logging out...\n");
        free(prefix);
        if (head)
            destroyAllPid(head);
        exit(EXIT_SUCCESS);

    inputDone:
        disableRawMode();
        printf("\n");
        strcpy(cmndCopy, inp);
        inp[strcspn(inp, "\n")] = 0;
        updateHistory(history, inp);

        formatInput(inp);
        fflush(stdin);
    }

    return 0;
}
