#include <string.h>

/******************************************************************************
 * BUFFER CONTROL
 *
 * TODO: These are very primitive and can be handled in a more
 * performant way that doesn't involve strlen().
 ******************************************************************************/

int buffer_append_char(char ch, char *buffer, int buffer_max)
{
        int len = strlen(buffer);
        if (len < buffer_max) {
                buffer[len++] = ch;
                buffer[len]   = 0;
        }
        return len;
}

int buffer_remove_char(int n, char *buffer)
{
        int len = strlen(buffer);
        while (n-->0) {
                if (len > 0) {
                        buffer[--len] = 0;
                } else {
                        break;
                }
        }
        return len;
}

int buffer_clear(char *buffer, int buffer_max)
{
        memset(buffer, 0, sizeof(char)*buffer_max);
        return 0;
}
