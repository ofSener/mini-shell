#ifndef PARSER_H
#define PARSER_H

// parse_line(): Girilen satırı, tırnak içi ifadeler, kaçış karakterleri vb. destekleyerek token'lara böler.
// Parametre: line - kullanıcı girişi, bg - eğer "&" komut sonundaysa 1 döner.
// Return: NULL ile biten token dizisi.
char **parse_line(char *line, int *bg);

// Token dizisi için ayrılan belleği serbest bırakır.
void free_tokens(char **tokens);

#endif
