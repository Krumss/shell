#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"

//keepBetween 1 = true , 0 = false
//if '|', keep = true
//if ' ', keep = false
char** sliceArg(char *cmd, char delim, int keepBetween, int max, int *numPt) {
    char **m_buffer = (char**)malloc((max + 1) * sizeof(char*));
    if (m_buffer == NULL) {
        fprintf(stderr, "3230shell: fail to allocate buffer.\n");
        //exit(EXIT_FAILURE);
        return NULL;
    }
    memset(m_buffer, 0, (max + 1) * sizeof(char*));
    char *cmdCopy = cmd; //copied cmd
    char *e; //string that include delim to the end
    //If you can depend on C99, use uintptr_t (defined in stdint.h)
    int index; // index of delim in string
    int num = 0; //num of sliced string

    while (1) {
        if (num > max) {
            fprintf(stderr, "3230shell: buffer overflow, max %d.\n", max);
        }
        
        e = strchr(cmdCopy, delim);
        //printf("CMDCOPY: %s", cmdCopy);
        if (e == NULL) {
            if (keepBetween == 0 && strcmp(cmdCopy, "") == 0) {
                num--;
                break;
            }
            
            m_buffer[num] = (char*)malloc(strlen(cmdCopy) * sizeof(char));
            memset(m_buffer[num], 0, strlen(cmdCopy) * sizeof(char));
            m_buffer[num] = cmdCopy;
            break;
            //return m_buffer;
        }
        
        index = (int)(e - cmdCopy);
        if(!(index == 0 && keepBetween == 0)) {
            m_buffer[num] = (char*)malloc((index + 1) * sizeof(char));
            memset(m_buffer[num], 0, (index + 1) * sizeof(char));

            strncpy(m_buffer[num], cmdCopy, index);
            num++;
        }
        cmdCopy = e + 1;

    }
    //printf("Num: %d", num);
    *numPt = num + 1;
    return m_buffer;
}