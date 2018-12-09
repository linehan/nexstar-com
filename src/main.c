#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <jdl/tty.h>
#include <jdl/time.h>
#include <jdl/textutils.h>
#include "buffer.h"
#include "nexstar.h"
#include "gl.h"
#include "serial.h"

struct nexstar_t telescope;

float RA  = 0.0;
float DEC = 0.0;


int end_program(void)
{
        if (serial_ready(&telescope.com)) {
                serial_close(&telescope.com);
        }
        return 0;
}

void nexstar_print(struct nexstar_t *nexstar)
{
        printg(-1, 0.98, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15,
                "     RA             DEC            ALT            AZ"
        );
        printg(-0.99, 0.96, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15,
                "hex  %-08X       %-08X       %-08X       %-08X",
                nexstar->ra.raw,
                nexstar->dec.raw,
                nexstar->alt.raw,
                nexstar->az.raw
        );
        printg(-0.99, 0.94, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15,
                "dec % -14d % -14d % -14d % -14d",
                nexstar->ra.raw,
                nexstar->dec.raw,
                nexstar->alt.raw,
                nexstar->az.raw
        );
        printg(-0.99, 0.92, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15,
                "deg % -14f % -14f % -14f % -14f",
                nexstar->ra.deg,
                nexstar->dec.deg,
                nexstar->alt.deg,
                nexstar->az.deg
        );
        printg(-0.99, 0.90, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15,
                "arc % -14f % -14f % -14f % -14f",
                nexstar->ra.arc,
                nexstar->dec.arc,
                nexstar->alt.arc,
                nexstar->az.arc
        );
        printg(-0.74f, 0.57f, 0.0f, 0.0f, 1.0f, GLUT_BITMAP_9_BY_15,
                "vernal equinox"
        );
}


void key_operations(void)
{
        if (key_is_pressed('q')) {
                end_program();
                exit(0);
        }
        if (key_is_pressed('h') && !key_is_handled('h')) {
                nexstar_signal(&telescope, NEXSTAR_SLEW_RA_NEG, 9);
        }
        if (key_is_pressed('j') && !key_is_handled('j')) {
                nexstar_signal(&telescope, NEXSTAR_SLEW_DEC_NEG, 9);
        }
        if (key_is_pressed('k') && !key_is_handled('k')) {
                nexstar_signal(&telescope, NEXSTAR_SLEW_DEC_POS, 9);
        }
        if (key_is_pressed('l') && !key_is_handled('l')) {
                nexstar_signal(&telescope, NEXSTAR_SLEW_RA_POS, 9);
        }
        if (key_is_pressed('y') && !key_is_handled('y')) {
                nexstar_signal(&telescope, NEXSTAR_SLEW_RA_NEG, 0);
        }
        if (key_is_pressed('u') && !key_is_handled('u')) {
                nexstar_signal(&telescope, NEXSTAR_SLEW_DEC_NEG, 0);
        }
        if (key_is_pressed('i') && !key_is_handled('i')) {
                nexstar_signal(&telescope, NEXSTAR_SLEW_DEC_POS, 0);
        }
        if (key_is_pressed('o') && !key_is_handled('o')) {
                nexstar_signal(&telescope, NEXSTAR_SLEW_RA_POS, 0);
        }
}


void display(void)
{
        static struct gl_circle lg = {.x=0.0f, .y=0.0f, .rotate=0.0f, .radius=0.8f};
        static struct gl_circle sm = {.x=0.0f, .y=0.0f, .rotate=0.0f, .radius=0.1f};

        key_operations();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   /* Set bg to black opaque */
        glClear(GL_COLOR_BUFFER_BIT);           /* clear the color buffer */

        gl_circle_dashed(&lg, 1.0f, 0.0f, 0.0f);
        gl_circle_solid(&sm, 1.0f, 0.0f, 0.0f);

        printg(-0.85,  0.00, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15, "180");
        printg( 0.81,  0.00, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15, "0");
        printg( 0.00,  0.81, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15, "90");
        printg( 0.00, -0.82, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15, "270");

        static float angle = 0.0f;
        /*float angle = telescope.ra.deg;*/

        angle += 0.1f;
        if (angle > 360.0f) {
                angle = 0.0f;
        }

        glBegin(GL_LINES);
                glColor3f(0.0f, 0.0f, 1.0f);
                float x = (float)0.11f * cos(angle * 3.14159/180.0f);
                float y = (float)0.11f * sin(angle * 3.14159/180.0f);
                glVertex2f(x, y);
                x = (float)0.8f * cos(0 * 3.14159/180.0f);
                y = (float)0.8f * sin(0 * 3.14159/180.0f);
                glVertex2f(x, y);
        glEnd();

        gl_draw_radius(&sm, angle, 0.0f, 0.0f, 0.1f);

        printg(-0.05, -0.05, 1.0f, 0.0f, 0.0f, GLUT_BITMAP_9_BY_15, "%f", angle);
        printg(-0.9, -0.9, 0.0f, 0.0f, 1.0f, GLUT_BITMAP_9_BY_15, "RA :%f  DEC:%f", RA, DEC);

        if (serial_ready(&telescope.com)) {
                nexstar_update(&telescope);
                nexstar_print(&telescope);
        }

        /* Render now */
        glFlush();
}





int main(int argc, char* argv[])
{
        gl_start(&argc, argv);

        gl_window("telescope", 0, 0, 300, 300);

        gl_loop_callback(display);
        gl_idle_callback(display);
        gl_capture_keyboard();

        serial_open(&telescope.com, "/dev/ttyUSB0");

        gl_loop_begin();

        return end_program();
}


