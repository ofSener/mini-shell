#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define MAX_LINE 1024
#define MAX_TOKENS 64

char **parse_line(char *line, int *bg) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    int pos = 0, i = 0;
    *bg = 0;  // Varsayılan olarak komut foreground çalışır.

    while (line[i]) {
        // Boşluk karakterlerini atla.
        while (line[i] && isspace(line[i])) i++;
        if (!line[i])
            break;

        char *token = malloc(MAX_LINE);  // Token için yeterli bellek ayır.
        int t = 0;

        // Tırnak içi ifade kontrolü: çift veya tek tırnak.
        if (line[i] == '"' || line[i] == '\'') {
            char quote = line[i++];
            while (line[i] && line[i] != quote) {
                if (line[i] == '\\' && line[i+1]) {
                    token[t++] = line[i+1];
                    i += 2;
                } else {
                    token[t++] = line[i++];
                }
            }
            if (line[i] == quote)
                i++;  // Kapanış tırnağını atla.
        } else {
            // Tırnaksız token
            while (line[i] && !isspace(line[i])) {
                if (line[i] == '\\' && line[i+1]) {
                    token[t++] = line[i+1];
                    i += 2;
                } else if (line[i] == '"' || line[i] == '\'') {
                    char quote = line[i++];
                    while (line[i] && line[i] != quote) {
                        if (line[i] == '\\' && line[i+1]) {
                            token[t++] = line[i+1];
                            i += 2;
                        } else {
                            token[t++] = line[i++];
                        }
                    }
                    if (line[i] == quote)
                        i++;
                } else {
                    token[t++] = line[i++];
                }
            }
        }
        token[t] = '\0';
        tokens[pos++] = token;
    }
    tokens[pos] = NULL;

    // Eğer son token "&" ise, komut arka planda çalışacak.
    if (pos > 0 && strcmp(tokens[pos-1], "&") == 0) {
        *bg = 1;
        free(tokens[pos-1]);
        tokens[pos-1] = NULL;
    }

    return tokens;
}

void free_tokens(char **tokens) {
    int i = 0;
    while(tokens[i]) {
        free(tokens[i]);
        i++;
    }
    free(tokens);
}
