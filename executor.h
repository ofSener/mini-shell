#ifndef EXECUTOR_H
#define EXECUTOR_H

// execute_command(): Built-in olmayan komutları fork() & execvp() kullanarak çalıştırır.
void execute_command(char **tokens, int bg);

// execute_pipeline(): Pipeline içeren komutları çalıştırır.
//   - commands: Pipeline'daki her segmentin token dizilerini içeren dizi
//   - num_commands: Pipeline'daki komut segment sayısı
//   - bg: Eğer 1 ise, pipeline arka planda çalıştırılacaktır.
void execute_pipeline(char ***commands, int num_commands, int bg);

#endif
