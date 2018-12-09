#ifndef _TELESCOPE_SERIAL_IO_H
#define _TELESCOPE_SERIAL_IO_H
#include <errno.h>
#include <termios.h>
#include <poll.h>
#include <jdl/list.h>
#include <jdl/fifo.h>
#include <stdarg.h>

struct serial_job {
        struct list_node node;
        int  next;
        char recv[256];
        int  code;
};

struct serial_com {
        struct list_t  jobs;
        struct termios save;
        struct termios conf;
        struct termios link;
        struct pollfd  poll[1];
        struct fifo_t  input;
        char   output[4096];
        int fd;
};


int serial_job(struct serial_com *com, int code, char *input);
struct serial_job *serial_run_jobs(struct serial_com *com);
int           serial_loop       (struct serial_com *com, char *in, char *out);
int           serial_open       (struct serial_com *com, const char *device_path);
int           serial_close      (struct serial_com *com);
int           serial_send       (struct serial_com *com, unsigned char c);
int           serial_send_string(struct serial_com *com, char *buffer, size_t size);
unsigned char serial_recv       (struct serial_com *com);
int           serial_ready      (struct serial_com *com);
int           serial_poll       (struct serial_com *com, int timeout_ms);

#endif
