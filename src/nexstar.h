#ifndef _NEXSTAR_H
#define _NEXSTAR_H
#include "serial.h"

#define NEXSTAR_MAX_LATENCY 3500 /* 3.5 seconds (3500 ms) */

enum job_code {
        NEXSTAR_RADEC,
        NEXSTAR_ALTAZ,
        NEXSTAR_SLEW_RA_POS,
        NEXSTAR_SLEW_RA_NEG,
        NEXSTAR_SLEW_DEC_POS,
        NEXSTAR_SLEW_DEC_NEG,
};

struct nexstar_unit {
        int raw;
        float deg;
        float arc;
};

struct nexstar_t {
        struct serial_com com;
        struct nexstar_unit az;
        struct nexstar_unit alt;
        struct nexstar_unit dec;
        struct nexstar_unit ra;
};


void nexstar_unit16(char *hex, struct nexstar_unit *unit);
void nexstar_unit32(char *hex, struct nexstar_unit *unit);
char *nexstar_read_until(struct nexstar_t *nexstar, int tok);
//int nexstar_read_radec(struct nexstar_t *nexstar);
//int nexstar_read_altaz(struct nexstar_t *nexstar);
int nexstar_set_altaz(struct nexstar_t *nexstar, int alt, int az);
int nexstar_set_radec(struct nexstar_t *nexstar, int ra, int dec);

int nexstar_update(struct nexstar_t *nexstar);
void nexstar_left(struct nexstar_t *nexstar);

int nexstar_signal(struct nexstar_t *nexstar, enum job_code code, int rate);

//int nexstar_update(struct nexstar_t *nexstar);

#endif
