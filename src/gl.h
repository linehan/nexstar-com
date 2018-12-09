#ifndef _MY_GL
#define _MY_GL
#include <stdarg.h>
#include <stdbool.h>
#include <GL/freeglut.h> 

struct gl_circle {
        float radius;
        float x;
        float y;
        float rotate;
};


void gl_window(const char *title, int x, int y, int w, int h);

void gl_capture_keyboard(void);

bool key_is_pressed(unsigned char key);
bool key_is_handled(unsigned char key);
void key_down      (unsigned char key, int x, int y);
void key_up        (unsigned char key, int x, int y);


int  printg(float x, float y, float r, float g, float b, void *font, char *fmt_str, ...);
void line_radius(float x, float y, float radius, float deg);
void line(float x1, float y1, float x2, float y2);
void circle(float x, float y, float radius);
void rect(float x, float y, float w, float h);

void gl_draw_radius(struct gl_circle *c, float a, float r, float g, float b);

void gl_circle_dashed(struct gl_circle *c, float r, float g, float b);
void gl_circle_solid(struct gl_circle *c, float r, float g, float b);


#define gl_start                glutInit
#define gl_stop() 
#define gl_loop_callback        glutDisplayFunc
#define gl_idle_callback        glutIdleFunc

#define gl_loop_begin           glutMainLoop


#endif
