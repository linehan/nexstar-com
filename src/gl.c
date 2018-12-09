#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <stdio.h> 
#include <math.h>
#include <stdarg.h>
#include <GL/freeglut.h> 
#include "gl.h"

#define PI 3.1415926f


void gl_window(const char *title, int x, int y, int w, int h)
{
        /* Create a window with some title */
        if (title == NULL) {
                glutCreateWindow("default"); 
        } else {
                glutCreateWindow(title);
        }

        /* Set the window's initial width and height */
        glutInitWindowSize(w, h);

        /* Position the window's initial top-left corner */
        glutInitWindowPosition(x, y); 
}



/*
 * GL_POINTS          Every vertex specified is a point.
 * GL_LINES           Every two vertices specified compose a line.
 * GL_LINE_STRIP      Every vertex specified after first two are connected.
 * GL_LINE_LOOP       The last vertex specified is connected to first vertex.
 * GL_TRIANGLES       Every three vertices specified compose a triangle.
 * GL_TRIANGLE_STRIP  Every vertex specified after first three vertices creates a triangle.
 * GL_TRIANGLE_FAN    Like GL_TRIANGLE_STRIP, except in fan shape.
 * GL_QUADS           Every four vertices specified compose a quadrilateral.
 * GL_QUAD_STRIP      Every two vertices specified after first four compose a connected quadrilateral.
 * GL_POLYGON         Polygon can be composed of as many sides as you want.
 */


int printg(float x, float y, float r, float g, float b, void *font, char *fmt_str, ...)
{
        va_list fmt_args;
        FILE   *memstream;
        char   *string;
        size_t  len;

        /* 
         * Open a new FILE stream. *strp will be dynamically 
         * allocated to contain characters written to the 
         * stream, and len will reflect these changes. 
         */
        if ((memstream = open_memstream(&string, &len)) == NULL) {
                return -1;
        }

        /* Write formatted output to stream */
        va_start(fmt_args, fmt_str);
        vfprintf(memstream, fmt_str, fmt_args);
        va_end(fmt_args);

        fflush(memstream);
        fclose(memstream);

        /*********************** GL ************************/

        glColor3f(r, g, b);
        glRasterPos2f(x, y);

        glutBitmapString(font, (unsigned char *)string);

        free(string);
        return len;
}


void gl_circle_dashed(struct gl_circle *c, float r, float g, float b)
{
        float x;
        float y;
        int   deg = 360;

        glBegin(GL_LINES);
                glColor3f(r, g, b);

                /*x = (float)c->radius * cos(deg * PI/180.0f);*/
                /*y = (float)c->radius * sin(deg * PI/180.0f);*/

                do { 
                        x = (float)c->radius * cos(deg * PI/180.0f);
                        y = (float)c->radius * sin(deg * PI/180.0f);
                        glVertex2f(x+c->x, y+c->y);
                } while (deg-->0);
        glEnd();
}

void gl_circle_solid(struct gl_circle *c, float r, float g, float b)
{
        float x;
        float y;
        int   deg = 360;

        glBegin(GL_LINES);
                glColor3f(r, g, b);
                x = (float)c->radius * cos(deg * PI/180.0f);
                y = (float)c->radius * sin(deg * PI/180.0f);
                do { 
                        glVertex2f(x+c->x, y+c->y);
                        x = (float)c->radius * cos(deg * PI/180.0f);
                        y = (float)c->radius * sin(deg * PI/180.0f);
                        glVertex2f(x+c->x, y+c->y);
                } while (deg-->0);
        glEnd();
}


void gl_draw_radius(struct gl_circle *c, float a, float r, float g, float b)
{
        glBegin(GL_LINES);
                glColor3f(r, g, b);
                glVertex2f(c->x, c->y);
                float x = (float)c->radius * cos(a+c->rotate * PI/180.0f);
                float y = (float)c->radius * sin(a+c->rotate * PI/180.0f);
                glVertex2f(x+c->x, y+c->y);
        glEnd();
}



void line_radius(float x, float y, float radius, float deg)
{
        glBegin(GL_LINES);
                glColor3f(0.0f, 0.0f, 1.0f);
                glVertex2f(x, y);
                x = (float)radius * cos(deg * PI/180.0f);
                y = (float)radius * sin(deg * PI/180.0f);
                glVertex2f(x, y);
        glEnd();
}


void line(float x1, float y1, float x2, float y2)
{
        glBegin(GL_LINES);
                glColor3f(0.0f, 0.0f, 1.0f);
                glVertex2f(x1, y1);
                glVertex2f(x2, y2);
        glEnd();
}


void circle(float x, float y, float radius)
{
        int j;
        glBegin(GL_LINES);
                glColor3f(1.0f,0.0f,0.0f);
                x = (float)radius * cos(359 * PI/180.0f);
                y = (float)radius * sin(359 * PI/180.0f);
                for (j=0; j<360; j++) {
                        glVertex2f(x, y);
                        x = (float)radius * cos(j * PI/180.0f);
                        y = (float)radius * sin(j * PI/180.0f);
                        glVertex2f(x, y);
                }
        glEnd();
}

void rect(float x, float y, float w, float h)
{
        /* Draw a Red 1x1 Square centered at origin */
        /* Each set of 4 vertices form a quad */
        glBegin(GL_QUADS);
                /* red */
                glColor3f(1.0f, 0.0f, 0.0f);
                /* x, y */
                glVertex2f(-1.0f*x, -1.0f*y);
                glVertex2f(      x, -1.0f*y);
                glVertex2f(      x,       y);
                glVertex2f(-1.0f*x,       y);
        glEnd();
}



/*
 * keyboard crap
 */

bool KEY_PRESSED[256]; 
bool KEY_HANDLED[256];




bool key_is_pressed(unsigned char key)
{
        return KEY_PRESSED[key];
}

bool key_is_handled(unsigned char key)
{
        if (KEY_HANDLED[key] == false) {
                KEY_HANDLED[key] = true;
        }
        return KEY_HANDLED[key];
}

void key_down(unsigned char key, int x, int y)
{
        KEY_PRESSED[key] = true;
        KEY_HANDLED[key] = false;
}

void key_up(unsigned char key, int x, int y)
{
        KEY_PRESSED[key] = false;
        KEY_HANDLED[key] = false;
}

void gl_capture_keyboard(void)
{
        glutKeyboardFunc(key_down); 
        glutKeyboardUpFunc(key_up); 
}


