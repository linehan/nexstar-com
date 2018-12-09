#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "serial.h"


int serial_job(struct serial_com *com, int code, char *input)
{
        struct serial_job *job;
        int i;
        int l;

        /* Make job */
        job       = calloc(1, sizeof(struct serial_job));
        job->code = code;
        job->next = 0;

        /* Add job to list */
        list_append(&com->jobs, job);

        /* Add input to send queue */
        for (i=0, l=strlen(input); i<l; i++) {
                fifo_enqueue(&com->input, input[i]);
        }

        return 1;
}


struct serial_job *serial_run_jobs(struct serial_com *com)
{
        struct serial_job *job;
        char i;
        char o;

        if (list_empty(&com->jobs)) {
                return NULL;
        } else {
                job = (struct serial_job *)list_head(&com->jobs);
        }

        /* WRITE TO THE SERIAL DEVICE */
        i = fifo_dequeue(&com->input);

        if (write(com->fd, &i, 1) < 0) {
                /* bad */
        } else {
                /*fprintf(stdout, "WROTE: %c\n", i);*/
        }

        /* READ FROM THE SERIAL DEVICE */
        if (read(com->fd, &o, 1) < 0) {
                /* bad */
        } else {
                /*fprintf(stdout, "READ: %c\n", o);*/
                job->recv[job->next++] = o;
                if (o == '#') {
                        job->recv[job->next] = 0;
                        list_remove_from(&com->jobs, job);
                        return job;
                }
        }

        return NULL;
}


int serial_open(struct serial_com *com, const char *device_path)
{
        list_init(&com->jobs);
        fifo_alloc(&com->input, 4096);
        fifo_reset(&com->input);

        /* Backup the current output stream settings */
        tcgetattr(STDOUT_FILENO, &com->save);

        memset(&com->conf, 0, sizeof(struct termios));
        memset(&com->link, 0, sizeof(struct termios));

        com->conf.c_iflag     = 0;
        com->conf.c_oflag     = 0;
        com->conf.c_cflag     = 0;
        com->conf.c_lflag     = 0;
        com->conf.c_cc[VMIN]  = 1;
        com->conf.c_cc[VTIME] = 0;

        tcsetattr(1, TCSANOW,   &com->conf);
        tcsetattr(1, TCSAFLUSH, &com->conf);

        com->link.c_iflag     = 0; /* input flags */
        com->link.c_oflag     = 0; /* output flags */
        /*
         * CREAD  allow reading
         * CLOCAL ignore modem controls
         * CS8    read 8 bit characters
         */
        com->link.c_cflag     = CS8|CREAD|CLOCAL;
        com->link.c_lflag     = 0;
        com->link.c_cc[VMIN]  = 1; /* read blocks */
        com->link.c_cc[VTIME] = 5; /* 0.5 seconds read timeout */

        com->fd = open(device_path, O_RDWR|O_NONBLOCK);

        if (com->fd < 0) {
                return -1;
        }

        /* 9600 baud output */
        cfsetospeed(&com->link, B9600);
        /* 9600 baud input */
        cfsetispeed(&com->link, B9600);

        tcsetattr(com->fd, TCSANOW, &com->link);

        return 1;
}


int serial_ready(struct serial_com *com)
{
        return fcntl(com->fd, F_GETFD) != -1 || errno != EBADF;
}



int serial_close(struct serial_com *com)
{
        close(com->fd);
        /* Restore output stream state */
        tcsetattr(1, TCSANOW, &com->save);
        return 0;
}


int serial_send(struct serial_com *com, unsigned char c)
{
        unsigned char l;
        if ((l = write(com->fd, &c, 1)) > 0) {
                return l;
        }
        return errno;
        /*
         * EAGAIN: Would block
         * EBADF : File is fucked
         * ENXIO : Request on nonexistent device
         * EIO   : Term is fucked
         */
}


int serial_send_string(struct serial_com *com, char *buffer, size_t size)
{
        int i;
        for (i=0; i<size; i++) {
                serial_send(com, (unsigned char)buffer[i]);
        }
        return 0;
}


unsigned char serial_recv(struct serial_com *com)
{
        unsigned char c;

        if (read(com->fd, &c, 1) > 0) {
                return c;
        }
        return errno;
        /*
         * EAGAIN: Would block
         * EBADF : File is fucked
         * ENXIO : Request on nonexistent device
         * EIO   : Term is fucked
         */
}



int serial_poll(struct serial_com *com, int timeout_ms)
{
        int poll_status;

        com->poll[0].fd     = com->fd;
        com->poll[0].events = POLLIN | POLLHUP | POLLNVAL;

        poll_status = poll(com->poll, 1, timeout_ms);

        if (poll_status == 0) {
                /* Timed out */
                return ETIMEDOUT;
        } else if (poll_status < 0) {
                /* Error */
                return -1;
        } else if (poll_status > 0) {
                if (com->poll[0].revents & POLLIN) {
                        return 1;
                } else if (com->poll[0].revents & POLLHUP) {
                        /* Hung up */
                        return EBADF;
                } else if (com->poll[0].revents & POLLNVAL) {
                        /* File descriptor is bad */
                        return EBADF;
                }
        }

        return 0;
}


