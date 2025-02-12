#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int execute_builtin(char **tokens) {
    if (tokens[0] == NULL)
        return 1;
    if (strcmp(tokens[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(tokens[0], "cd") == 0) {
        if (tokens[1] == NULL) {
            fprintf(stderr, "cd: argument required\n");
        } else {
            if (chdir(tokens[1]) != 0)
                perror("cd error");
        }
        return 1;
    }
    return 0;
}
