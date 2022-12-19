**AABTLTMTSH**:Ambitious aspirations but too little time makes this shell hacky 

## For best results, turn OFF font ligatures
## It is NECESSARY for your compiler to support the `once` pragma directive (supported on latest gcc version 12.2)
## Discover searches hidden files and folders as well

## To run:  
To compile : `make`  
To remove object files : `make clean`

To run : `./shell`

### File structure:  
.  
├── README.md  
├── bg.c  
├── bg.h  
├── cd.c  
├── cd.h  
├── discover.c  
├── discover.h  
├── echo.c  
├── echo.h  
├── fg.c  
├── fg.h  
├── headers.h  
├── history.c  
├── history.h  
├── listDir.c  
├── listDir.h  
├── main.c  
├── makefile  
├── pinfo.c  
├── pinfo.h  
├── prompt.c  
├── prompt.h  
├── pwd.c  
├── pwd.h  
└── variables.h  

**bg.c** and **bg.h**  
Implements the functions needed to make a background process and store its pid in a linked list. It also implements the function that given a pid of a background process that has exited prints its return code and removes it from the list.

**cd.c** and **cd.h**  
Implements the cd function. Supports absolute and relative paths along with `.`, `..`, `~`, `-` and null. `getAbsPath` converts a path of the form `~/` to an absolute path that can be used to change directory using `chdir`. `changeDirect` calls `chdir` and `changeDirectoryUpdatePrefix` changes the directory and modifies the prefix(prompt) to the appropriate directory.  

**discover.c** and **discover.h**  
Contains the function that parses input and the two functions that searches for a file and one that prints all files and directories. It does not consider . and .. and prints the directories and files if they exist.

**echo.c** and **echo.h**  
Implements the echo function. Quotes, escape sequences and multiline strings are not supported. Multiple spaces and tabs are truncated to a single space.

**fg.c** and **fg.h**  
Has function to create and execute a foreground process and return the time it has executed for. The time calculated is the time taken by the last executed foreground process.

**history.c** and **history.h**  
Contains the functions required to store command history. The entire line is stored in history, except if it is exactly the same as the previous command. It is first stored locally in a circular array and then on exiting the shell it is stored in a file for future reference. On startup of the shell, the history file is attempted to be read and the stored history is loaded onto the program.

**listDir.c** and **listDir.h**  
Contains functions that implements the `ls` and `ls -l` commands and a function that parses the flag for `-l` and calls the appropriate function.

**main.c**  
Contains setting up of global variables, first call to print prompt, processes string inputs and calls appropriate functions based on input.  

**headers.h**  
Contains the `#pragma once` directive to prevent multiple includes. Includes local headers and system library headers. Defines `PATH_MAX`, the longest path length to 4096 if undefined(usually in `sys/limits.h`).  
Includes the system libraries and local headers needed.

**pinfo.c** and **pinfo.h**  
Contains functions to print the pinfo for the current process and for a process with a given pid. Reads the `/proc/pid/stat` file.

**variables.h**  
Contains global variables and definitions for terminal colours RED, GREEN, BLUE and RESET.

**prompt.c** and **prompt.h**  
The names prefix and prompt are used interchangeably. `getPrefix` updates appropriate part of the prefix according to the parameter passed. See `prompt.h` for more details.

**pwd.c** and **pwd.h**  
Implements the pwd function. `getCurrentDirectory` returns a malloced string while `printWorkingDirectory` automatically allocates, prints and frees.

**makefile**  
`make` to build executable, `make clean` to remove object files.