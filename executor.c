#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

void execute_command(char **tokens, int bg) {
    pid_t pid = fork();
    if (pid == 0) {  // Child process
        // Child process'te SIGINT’i varsayılan ayara döndür.
        signal(SIGINT, SIG_DFL);
        // Çıktı yönlendirmesi varsa, uygula.
        handle_output_redirection(tokens);
        if (execvp(tokens[0], tokens) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork error");
    } else {
        // Parent process: foreground komutlarda child tamamlanana kadar bekle, arka planda PID bildir.
        if (!bg) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            printf("[Background] PID: %d\n", pid);
        }
    }

}
void handle_output_redirection(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], ">") == 0) {
            // ">" bulundu, sonraki token hedef dosya adıdır.
            if (tokens[i+1] == NULL) {
                fprintf(stderr, "Redirection error: no output file specified.\n");
                exit(EXIT_FAILURE);
            }
            int fd = open(tokens[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open redirection error");
                exit(EXIT_FAILURE);
            }
            // STDOUT'u fd'ye yönlendir.
            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("dup2 redirection error");
                exit(EXIT_FAILURE);
            }
            close(fd);
            // ">" ve dosya adını token dizisinden kaldırıyoruz.
            tokens[i] = NULL;
            break;
        }
    }
}
// Pipeline desteği: Çoklu komut segmentini birbirine pipe ile bağlar.
void execute_pipeline(char ***commands, int num_commands, int bg) {
    int i, j;
    int num_pipes = num_commands - 1;
    int pipefds[2 * num_pipes]; // Her pipe 2 dosya tanımlayıcısı içerir

    // Gerekli tüm pipe'ları oluştur
    for (i = 0; i < num_pipes; i++) {
        if (pipe(pipefds + i*2) < 0) {
            perror("pipe error");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid;
    for (i = 0; i < num_commands; i++) {
        pid = fork();
        if (pid == 0) {  // Child process
            // Eğer ilk komut değilse, önceki pipe'ın okuma ucunu STDIN'e yönlendir.
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("dup2 error");
                    exit(EXIT_FAILURE);
                }
            }
            // Eğer son komut değilse, mevcut pipe'ın yazma ucunu STDOUT'a yönlendir.
            if (i < num_commands - 1) {
                if (dup2(pipefds[i*2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2 error");
                    exit(EXIT_FAILURE);
                }
            }
            // Tüm pipe dosya tanımlayıcılarını kapat.
            for (j = 0; j < 2 * num_pipes; j++) {
                close(pipefds[j]);
            }
            // Komutu çalıştır.
            if (execvp(commands[i][0], commands[i]) < 0) {
                perror("execvp error in pipeline");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("fork error in pipeline");
            exit(EXIT_FAILURE);
        }
        // Parent döngüye devam ediyor.
    }
    // Parent: Tüm pipe tanımlayıcılarını kapat.
    for (i = 0; i < 2 * num_pipes; i++) {
        close(pipefds[i]);
    }
    // Foreground çalışıyorsa tüm child process'lerin bitmesini bekle.
    if (!bg) {
        for (i = 0; i < num_commands; i++) {
            wait(NULL);
        }
    } else {
        printf("[Background] Pipeline executed.\n");
    }
}
