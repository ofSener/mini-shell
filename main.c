#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "builtins.h"
#include "executor.h"
#include "signals.h"

#define MAX_LINE 1024
#define MAX_PIPE_CMDS 10  // Pipeline'daki maksimum komut sayısı

// Yardımcı: Bir string'in başındaki ve sonundaki boşlukları temizler.
char *trim_whitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = '\0';
    return str;
}

int main() {
    setup_signal_handlers();
    char *line = NULL;
    size_t len = 0;
    while (1) {
        printf("myshell> ");
        fflush(stdout);
        if (getline(&line, &len, stdin) == -1) {
            break;
        }
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0)
            continue;

        int bg = 0;
        // Pipeline var mı kontrol et: '|' karakteri varsa.
        if (strchr(line, '|') != NULL) {
            // Eğer komut satırı sonuna '&' eklenmişse, arka plan bayrağını ayarla.
            char *ampersand = strrchr(line, '&');
            if (ampersand != NULL) {
                *ampersand = '\0';
                bg = 1;
            }
            // Pipeline segmentlerini '|' karakteriyle ayır.
            char *pipe_segments[MAX_PIPE_CMDS];
            int cmd_count = 0;
            char *segment = strtok(line, "|");
            while (segment != NULL && cmd_count < MAX_PIPE_CMDS) {
                pipe_segments[cmd_count++] = trim_whitespace(segment);
                segment = strtok(NULL, "|");
            }
            // Her segmenti token dizisine dönüştür.
            char **commands[MAX_PIPE_CMDS];
            for (int i = 0; i < cmd_count; i++) {
                int dummy_bg = 0;
                commands[i] = parse_line(pipe_segments[i], &dummy_bg);
            }
            // Pipeline'ı çalıştır.
            execute_pipeline(commands, cmd_count, bg);
            // Her segment için ayrılan bellekleri serbest bırak.
            for (int i = 0; i < cmd_count; i++) {
                free_tokens(commands[i]);
            }
            continue;
        }

        // Pipeline yoksa, tek komut olarak işle.
        char **tokens = parse_line(line, &bg);
        if (tokens[0] == NULL) {
            free_tokens(tokens);
            continue;
        }
        if (execute_builtin(tokens)) {
            free_tokens(tokens);
            continue;
        }
        execute_command(tokens, bg);
        free_tokens(tokens);
    }
    free(line);
    return 0;
}
