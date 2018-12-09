#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <jdl/tty.h>
#include <jdl/time.h>
#include "buffer.h"
#include "nexstar.h"
#include "serial.h"

/******************************************************************************
 * PARAMETERS
 ******************************************************************************/
#define BUFFER_SIZE  255
#define BUFFER_ALLOC 256

/******************************************************************************
 * MAIN
 ******************************************************************************/

int main(void)
{
        char buffer[BUFFER_ALLOC];
        int  length;

        WINDOW *body;
        WINDOW *head;
        WINDOW *foot;

        struct nexstar_t nexstar;
        bool draw_transmit = false;
        bool draw_response = false;
        char input  = 0;
        char output = 0;
        int  cursor = 0;
        int ra  = 0;
        int dec = 0;
        int alt = 0;
        int az  = 0;

        buffer_clear(buffer, BUFFER_SIZE);

        /*
         * Set up the ncurses environment
         * and print the title screen.
         */

        nc_start("auto");
        nc_colorpair(1, COLOR_MAGENTA, -1);

        head = newwin(5, COLS, 0, 0);
        body = newwin(LINES-6, COLS, 5, 0);
        foot = newwin(1, COLS, LINES-1, 0);

        do {
                mvwprintw(body, NC_CENTER(-8, 0, "Telescope Control\n"));
                mvwprintw(body, NC_CENTER(-6, 0, "Press any key to continue\n"));
                wrefresh(body);
                sleep_ns((long)5000000);
        } while ((input = getch()) == ERR);

        wclear(body);
        wclear(head);
        wrefresh(body);
        wrefresh(head);

        /*
         * Start the main execution by opening the
         * serial device and entering the main loop.
         */

        serial_open(&nexstar.com, "/dev/ttyUSB0");

        /********************************************
         * MAIN LOOP
         ********************************************/

        while ((input = getch())) {

                switch ((int)input) {
                /* There is no input */
                case ERR:
                case 0:
                        break;

                /* Quit the program */
                case 'q':
                        goto done;
                        break;

                /* Backspace in the buffer */
                case KEY_BACKSPACE:
                case KEY_DC:
                case 127:
                        length = buffer_remove_char(1, buffer);
                        break;

                /* Transmit the buffer */
                case '\n':
                        serial_send_string(&nexstar.com, buffer, length);
                        length = buffer_clear(buffer, BUFFER_SIZE);
                        draw_transmit = true;
                        draw_response = true;
                        break;

                case 'B':
                        sscanf(buffer, "%d,%d", &alt, &az);
                        wprintw(body, "SET ALT:%d AZ:%d\n", alt, az);
                        length = buffer_clear(buffer, BUFFER_SIZE);
                        nexstar_set_altaz(&nexstar, alt, az);
                        draw_transmit = true;
                        draw_response = true;
                        break;

                /* Add a character to the buffer */
                default:
                        draw_transmit = false;
                        length = buffer_append_char(input, buffer, BUFFER_SIZE);
                        break;
                }

                /*
                 * If new data is available on the serial
                 * port, print it out.
                 */
                if (draw_response == true) {
                        do {
                                output = serial_recv(&nexstar.com);
                                switch (output) {
                                case EAGAIN:
                                        break;
                                case ENXIO: /* non-existent device */
                                case EBADF: /* bad file descriptor */
                                case EIO:   /* physical I/O error  */
                                        goto done;
                                default:
                                        wprintw(body, "%c", output);
                                        break;
                                }
                        } while (output != '#');
                        wprintw(body, "\n");
                        draw_response = false;
                }

                wclear(head);
                wclear(foot);

                if (length == 0) {
                        mvwprintw(foot, 0, 0, ": ");
                } else {
                        mvwprintw(foot, 0, 0, ": %s", buffer);
                }

                if (draw_transmit) {
                        mvwprintw(foot, 0, COLS-5, "SENT");
                } else {
                        mvwprintw(foot, 0, COLS-5, "    ");
                }

                cursor = A_CHARTEXT & mvwinch(foot, 0, 2+(length));
                mvwaddch(foot, 0, 2+(length), cursor|COLOR_PAIR(1)|A_REVERSE);

                if (nexstar_read_radec(&nexstar) == -1) {
                        goto done;
                }
                if (nexstar_read_altaz(&nexstar) == -1) {
                        goto done;
                }

                mvwprintw(head, 0, COLS-63, "     RA             DEC            ALT            AZ");

                mvwprintw(head, 1, COLS-63, "hex  %-08X       %-08X       %-08X       %-08X",
                        nexstar.ra.raw,
                        nexstar.dec.raw,
                        nexstar.alt.raw,
                        nexstar.az.raw
                );

                mvwprintw(head, 2, COLS-63, "dec % -14d % -14d % -14d % -14d",
                        nexstar.ra.raw,
                        nexstar.dec.raw,
                        nexstar.alt.raw,
                        nexstar.az.raw
                );

                mvwprintw(head, 3, COLS-63, "deg % -14f % -14f % -14f % -14f",
                        nexstar.ra.deg,
                        nexstar.dec.deg,
                        nexstar.alt.deg,
                        nexstar.az.deg
                );

                mvwprintw(head, 4, COLS-63, "arc % -14f % -14f % -14f % -14f",
                        nexstar.ra.arc,
                        nexstar.dec.arc,
                        nexstar.alt.arc,
                        nexstar.az.arc
                );

                wrefresh(head);
                wrefresh(foot);
                wrefresh(body);

                sleep_ns((long)5000000);
        }

done:
        serial_close(&nexstar.com);
        nc_stop();

        return 1;
}


