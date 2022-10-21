#pragma once

#include "stdlib.h"
#include "string.h" //strtok
#include "unistd.h"
#include "job.h"

char* getCmd();

Job* cmdToJob(char* cmd);