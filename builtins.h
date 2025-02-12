#ifndef BUILTINS_H
#define BUILTINS_H

// execute_builtin(): exit, cd etc.
// Return: if command is built-in  1 else 0.
int execute_builtin(char **tokens);

#endif
