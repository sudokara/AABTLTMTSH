#pragma once

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <string.h>
#include <sys/utsname.h>
#include <signal.h>
#include <termios.h>
#include <sys/stat.h>

#include "cd.h"
#include "prompt.h"
#include "pwd.h"
#include "variables.h"
#include "echo.h"
#include "listDir.h"
#include "history.h"
#include "pinfo.h"
#include "fg.h"
#include "bg.h"
#include "discover.h"
#include "input.h"
#include "jobs.h"

pid_t Pid = -1;
int waitFg = 1;
