#include "history.h"

struct historyStack* makeHistoryStack(int capacity)
{
    // initialise struct for history
    struct historyStack *history = malloc(sizeof(struct historyStack));
    history->capacity = capacity;
    history->top = -1;
    history->arr = calloc(capacity, sizeof(char *));

    return history;
}

void updateHistory(struct historyStack* history, char *command)
{
    if (!command || !command[0]) // no command
        return;
    if (history->top != -1 && history->arr[history->top]) // current command same as last in history
        if (!strcmp(command, history->arr[history->top]))
            return;

    history->top = (history->top + 1) % history->capacity;
    
    if (history->arr[history->top]) // free earliest history and replace with current command
    {
        free(history->arr[history->top]);
        history->arr[history->top] = malloc(strlen(command + 1));
        if (history->arr[history->top] == NULL)
        {
            perror(RED"Could not update history: "RESETCOL);
            return;
        }
        strcpy(history->arr[history->top], command);
    }
    else // add command to history list
    {
        history->arr[history->top] = malloc(strlen(command + 1));
        if (history->arr[history->top] == NULL)
        {
            perror(RED"Could not update history: "RESETCOL);
            return;
        }
        strcpy(history->arr[history->top], command);
    }
}

void fetchHistory(struct historyStack* history, char *pathToHistory)
{
    FILE *historyFile = fopen(pathToHistory, "r"); // read history from file
    if (historyFile == NULL)
    {
        // perror("Could not retrieve stored history");
        return;
    }

    char buffer[PATH_MAX + 20];

    int i = 0;
    while (i < history->capacity && fscanf(historyFile, "%[^\n]s", buffer) != -1) // read each line from the history file
    {
        char *temp = malloc(sizeof(buffer) + 1);
        strcpy(temp, buffer);
        if (history->arr[++(history->top)])
            free(history->arr[history->top]);

        history->arr[history->top] = temp;
        char ch;
        fscanf(historyFile, "%c", &ch);
        i++;
    }

    fclose(historyFile);
    return;
}

void storeHistory(struct historyStack* history, char *pathToHistory)
{
    FILE *historyFile = fopen(pathToHistory, "w"); // open file to store history
    if (historyFile == NULL)
    {
        perror(RED"Could not store history"RESETCOL);
        return;
    }

    if (history->top != -1) // store history in file
    {
        int count = 0;
        int i = (history->top + 1) % history->capacity;
        do
        {
            if (history->arr[i])
            {
                fprintf(historyFile, "%s\n", history->arr[i]);
                free(history->arr[i]);
            }

            i = (i + 1) % history->capacity;
            count++;
        } while (count != history->capacity);
    }
    free(history->arr);
    free(history);

    fclose(historyFile);
    return;
}

void printHistory(struct historyStack *history)
{
    if (history->top != -1)
    {
        int count = 0;
        int i = (history->top + 1) % history->capacity;
        do
        {
            if (history->arr[i] && count >= 10)
                printf("%s\n", history->arr[i]);

            i = (i + 1) % history->capacity;
            count++;
        } while (count != 20);
    }
}