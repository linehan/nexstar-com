#ifndef _BUFFER_H
#define _BUFFER_H

#define BUFFER_LENGTH 4096

int buffer_append_char(char ch, char *buffer, int buffer_max);
int buffer_remove_char(int n, char *buffer);
int buffer_clear(char *buffer, int buffer_max);

#endif
