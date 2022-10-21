#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"
#include "util.h"
#include "constants.h"

char* getCmd() {
    printf("$$ 3230shell ##   ");
    // Allocate read buffer.
    char* buffer = (char*)malloc(MAX_SIZE_CMD*sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "3230shell: fail to allocate buffer.\n");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, MAX_SIZE_CMD*sizeof(char));

    char c;
    int pos = 0;

    // Read loop.
    while (1) {
        c = getchar();
        // Return read buffer if reach newline or EOF.
        if (c == EOF || c == '\n') {
            if (c == EOF)
                printf("\n");
            buffer[pos] = '\0';
            return buffer;
        } else {
            // Append character to read buffer.
            buffer[pos++] = c;
        }

        // Raise error if input length exceeds max buffer size.
        if (pos >= MAX_SIZE_CMD) {
            fprintf(stderr, "3230shell: buffer overflow, max 1024 char.\n");
            exit(EXIT_FAILURE);
        }
    }
}

Job* cmdToJob(char* cmd) {
    if (cmd == NULL || strcmp(cmd, "") == 0)
        return NULL;
    //break the line into commands seperated by "|"
    char *processArgPtr;
    char **p_argvs = (char**)malloc(MAX_PROCESS_NUM * sizeof(char*));
    memset(p_argvs, 0, MAX_PROCESS_NUM * sizeof(char*));
    int processNum;

    p_argvs = sliceArg(cmd, '|', 1, MAX_PROCESS_NUM, &processNum);

    /*printf("Size of o_argvs: %d, %d, %d\n", (int)(sizeof(p_argvs) / sizeof(p_argvs[0])), (int)sizeof(p_argvs), (int)sizeof(p_argvs[0]));
    printf("String: %s", p_argvs[0]);*/


    Job* j = (Job*)malloc(sizeof(Job));
    if (j == NULL) {
        fprintf(stderr, "3230shell: fail to allocate job buffer.\n");
        return NULL;
    }
    memset(j, 0, sizeof(Job));
    j->stdin = STDIN_FILENO;
    j->stdout = STDOUT_FILENO;

    Process* head = NULL;
    Process* prev = NULL;
    Process* p;
    printf("Process Num: %d\n", processNum);
    //printf("String: %s, %s\n", p_argvs[0], p_argvs[1]);
    for (int i = 0; i < processNum; i++) {
        //p_argv to argv
        char **argv = (char**)malloc(MAX_STRING_LENGTH * sizeof(char*));
        memset(argv, 0, MAX_STRING_LENGTH * sizeof(char*));
        int argSize = 0;
        argv = sliceArg(p_argvs[i], ' ', 0, MAX_STRING_LENGTH, &argSize);
        //printf("argv: \'%s\'", argv[0]);
        if (argSize == 0) {
            if (processNum == 1) {
                return NULL;
            } else {
                printf("3230shell: should not have two consecutive | without in-between command\n");
                return NULL;
            }
            
        }
        
        //printf("argSize: %d\n", argSize);
        p = (Process*) malloc(sizeof(Process));
        //if p == NULL
        memset(p, 0, sizeof(Process));
        p->argv = (char**)malloc((argSize + 1) * sizeof(char*));
        memset(p->argv, 0, (argSize + 1) * sizeof(char*));
        for (size_t j = 0; j < argSize; j++) {
            p->argv[j] = argv[j];
        }
        p->argv[argSize] = NULL;
        if (head == NULL) {
            head = p;
            prev = p;
        } else {
            prev->next = p;
            prev = p;
        }
        
    }
    j->head = head;
    if (j->head == NULL)
    {
        //cleanJob(j)
        return NULL;
    }
    
    return j;
}