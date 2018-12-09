#ifndef _JLIB_TTY_H
#define _JLIB_TTY_H

/* Extended character support (must precede #includes) */
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif
#include <locale.h>
#include <wchar.h>
#include <ncurses.h>
#include <panel.h>

#if !defined(NCURSES_VERSION)
#error "Missing ncurses headers; you may need to install ncurses-devel"
#endif

/*
 * nc_start(): If you want ncurses to be
 * loaded with "reasonable" defaults
 * automatically.
 */
#define NC_REASONABLE_DEFAULTS  "auto"
/*
 * nc_start(): If you want ncurses to be
 * loaded raw, so that it can be configured
 * explicitly.
 */
#define NC_CONFIGURE            NULL


#define NC_INPUT_BUFFER         (1 << 0)
#define NC_INPUT_ECHO           (1 << 1)
#define NC_INPUT_NONBLOCK       (1 << 2)
#define NC_INPUT_KEYPAD         (1 << 3)
#define NC_CURSOR               (1 << 4)
#define NC_COLOR                (1 << 5)
#define NC_COLOR_DEFAULTS       (1 << 6)
#define NC_COLOR_PROGRAMMABLE   (1 << 7)
#define NC_COLOR_256            (1 << 8)


int nc_start  (const char *option);
int nc_stop   (void);
int nc_set    (bool value, int option);
int nc_waitkey(long ns);

int nc_colorpair(char pairno, char fg, char bg);

#define NC_CENTER(y, x, str) \
        ((LINES/2)+(y)), (((COLS/2)-((strlen((str))-1)/2))+(x)), str

//cchar_t *cch(wchar_t *wch, attr_t attr, short co);
//void win_background(WINDOW *win, cchar_t *cch);
//void win_background_set(WINDOW *win, cchar_t *cch);

#ifndef pan_refresh
#define pan_refresh()    \
        update_panels()
#endif

#ifndef scr_refresh
#define scr_refresh()    \
        update_panels(); \
        doupdate()
#endif

#ifndef win_refresh
#define win_refresh(win) \
        wrefresh(win)
#endif


/**
 * The ESC key code is somewhat implementation-defined, and ncurses
 * doesn't define a macro for it as such, although it supports many
 * other special keys. '27' is common enough for now, just don't forget!
 */
#ifndef KEY_ESC
#define KEY_ESC 27
#endif


#endif
