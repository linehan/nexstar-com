#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "buffer.h"
#include "serial.h"
#include "nexstar.h"


/******************************************************************************
 * NEXSTAR CONSTANTS
 *
 * The Nexstar protocol delivers coordinate values as
 * high-precision (32-bit) or low-precision (16-bit).
 *
 ******************************************************************************/

#define NEXSTAR_LEN16 5
#define NEXSTAR_LEN32 9

/*
 * Maximum values of the low-precision (16-bit)
 * and high-precision (32-bit) parameters.
 */
#define NEXSTAR_MAX16       (65536)
#define NEXSTAR_MAX32       (4294967296)
#define NEXSTAR_MAX16_FLOAT (65536.0f)
#define NEXSTAR_MAX32_FLOAT (4294967296.0f)

/*
 * Geometrical constants for conversion between
 * degrees, minutes, and seconds of arc.
 */
#define DEGREE_IN_CIRCLE    (360.0f)
#define ARCMIN_IN_DEGREE    (60.0f)
#define ARCSEC_IN_DEGREE    (3600.0f)
#define ARCMIN_IN_CIRCLE    (21600.0f)   // DEGREE_IN_CIRCLE*ARCMIN_IN_DEGREE
#define ARCSEC_IN_CIRCLE    (1296000.0f) // DEGREE_IN_CIRCLE*ARCSEC_IN_DEGREE


/******************************************************************************
 * NEXSTAR CONVERSIONS
 *
 * Nexstar protocol values are frequently converted
 * to/from suitable equivalents in either degrees of
 * a circle or seconds of arc.
 *
 ******************************************************************************/

/*
 * NEXSTAR => DEGREES
 */
#define NEXSTAR_16BIT_TO_DEGREE(r) \
        (float) (((float)(r) / NEXSTAR_MAX16_FLOAT) * DEGREE_IN_CIRCLE)

#define NEXSTAR_32BIT_TO_DEGREE(r) \
        (float) (((float)(r) / NEXSTAR_MAX32_FLOAT) * DEGREE_IN_CIRCLE)

/*
 * NEXSTAR => ARCSECONDS
 */
#define NEXSTAR_16BIT_TO_ARCSEC(r) \
        (float) (NEXSTAR_16BIT_TO_DEGREE((r)) * ARCSEC_IN_DEGREE)

#define NEXSTAR_32BIT_TO_ARCSEC(r) \
        (float) (NEXSTAR_32BIT_TO_DEGREE((r)) * ARCSEC_IN_DEGREE)

/*
 * DEGREES => NEXSTAR
 */
#define NEXSTAR_DEGREE_TO_16BIT(x) \
        (float) (((float)(x) / DEGREE_IN_CIRCLE) * NEXSTAR_MAX16_FLOAT)

#define NEXSTAR_DEGREE_TO_32BIT(x) \
        (float) (((float)(x) / DEGREE_IN_CIRCLE) * NEXSTAR_MAX32_FLOAT)

/*
 * ARCSECONDS => NEXSTAR
 */
#define NEXSTAR_ARCSEC_TO_16BIT(x) \
        (float) (((float)(x) / ARCSEC_IN_CIRCLE) * NEXSTAR_MAX16_FLOAT)

#define NEXSTAR_ARCSEC_TO_32BIT(x) \
        (float) (((float)(x) / ARCSEC_IN_CIRCLE) * NEXSTAR_MAX32_FLOAT)


/******************************************************************************
 * NEXSTAR PARSE
 *
 * Nexstar protocol values are transmitted in hexadecimal
 * and must be converted to an equivalent base-10 integer
 * before being used in calculations.
 *
 *       low-precision (16-bit)  XXXX,XXXX#
 *      high-precision (32-bit)  XXXXXXXX,XXXXXXXX#
 *
 ******************************************************************************/

static inline
int nexstar_parse_hex16(char *input, int *a, int *b)
{
        if (input==NULL || a==NULL || b==NULL || strlen(input) != 10) {
                return 0;
        }

        static char buf[5];

        buf[0] = input[0];
        buf[1] = input[1];
        buf[2] = input[2];
        buf[3] = input[3];
        /* ,     input[4] */
        buf[4] = 0;

        *a = (int)strtol(buf, NULL, 16);

        buf[0] = input[5];
        buf[1] = input[6];
        buf[2] = input[7];
        buf[3] = input[8];
        /* #     input[9] */
        buf[4] = 0;

        *b = (int)strtol(buf, NULL, 16);

        return 1;
}

static inline
int nexstar_parse_hex32(char *input, int *a, int *b)
{
        if (input==NULL || a==NULL || b==NULL || strlen(input) != 18) {
                return 0;
        }

        static char buf[9];

        buf[0] = input[0];
        buf[1] = input[1];
        buf[2] = input[2];
        buf[3] = input[3];
        buf[4] = input[4];
        buf[5] = input[5];
        buf[6] = input[6];
        buf[7] = input[7];
        /* ,     input[8] */
        buf[8] = 0;

        *a = (int)strtol(buf, NULL, 16);

        buf[0] = input[9];
        buf[1] = input[10];
        buf[2] = input[11];
        buf[3] = input[12];
        buf[4] = input[13];
        buf[5] = input[14];
        buf[6] = input[15];
        buf[7] = input[16];
        /* #     input[17] */
        buf[8] = 0;

        *b = (int)strtol(buf, NULL, 16);

        return 1;
}



void nexstar_unit16(char *hex, struct nexstar_unit *unit)
{
        if (hex != NULL && unit != NULL) {
                unit->raw = (int)strtol(hex, NULL, 16);
                unit->deg = NEXSTAR_16BIT_TO_DEGREE(unit->raw);
                unit->arc = NEXSTAR_16BIT_TO_ARCSEC(unit->raw);
        }
}

void nexstar_unit32(char *hex, struct nexstar_unit *unit)
{
        if (hex != NULL && unit != NULL) {
                unit->raw = (int)strtol(hex, NULL, 16);
                unit->deg = NEXSTAR_32BIT_TO_DEGREE(unit->raw);
                unit->arc = NEXSTAR_32BIT_TO_ARCSEC(unit->raw);
        }
}


static inline void to_hex(char *buffer, int value)
{
        sprintf(buffer, "%04X", (unsigned char)value & 0xFFFFU);
}



int nexstar_set_altaz(struct nexstar_t *nexstar, int alt, int az)
{
        char buffer[256];

        buffer_clear(buffer, 256);
        sprintf(buffer, "B%04X,%04X", az, alt);
        serial_send_string(&nexstar->com, buffer, 10);

        return 1;
        /*return nexstar_wait_ack(nexstar);*/
}

int nexstar_set_radec(struct nexstar_t *nexstar, int ra, int dec)
{
        char buffer[256];

        buffer_clear(buffer, 256);
        sprintf(buffer, "R%04X,%04X", ra, dec);
        serial_send_string(&nexstar->com, buffer, 10);

        /*serial_recv(&nexstar->com);*/
        return 1;
}


void parse_2x4(char *buffer, char *buf1, char *buf2)
{
        if (strlen(buffer) == 10) {
                buf1[0] = buffer[0];
                buf1[1] = buffer[1];
                buf1[2] = buffer[2];
                buf1[3] = buffer[3];
                /* ,      buffer[4] */
                buf2[0] = buffer[5];
                buf2[1] = buffer[6];
                buf2[2] = buffer[7];
                buf2[3] = buffer[8];
                /* #      buffer[9] */
        } else {
                printf("BAD LEN:%d\n", strlen(buffer));
        }
}

void parse_2x8(char *buffer, char *buf1, char *buf2)
{
        if (strlen(buffer) == 18) {
                buf1[0] = buffer[0];
                buf1[1] = buffer[1];
                buf1[2] = buffer[2];
                buf1[3] = buffer[3];
                buf1[4] = buffer[4];
                buf1[5] = buffer[5];
                buf1[6] = buffer[6];
                buf1[7] = buffer[7];
                buf1[8] = 0;
                /* ,      buffer[8] */
                buf2[0] = buffer[9];
                buf2[1] = buffer[10];
                buf2[2] = buffer[11];
                buf2[3] = buffer[12];
                buf2[4] = buffer[13];
                buf2[5] = buffer[14];
                buf2[6] = buffer[15];
                buf2[7] = buffer[16];
                buf2[8] = 0;
                /* #      buffer[17] */
        } else {
                printf("BAD LEN:%d\n", strlen(buffer));
        }
}



int nexstar_signal(struct nexstar_t *nexstar, enum job_code code, int rate)
{
        static char buffer[4096];

        switch (code) {
        case NEXSTAR_SLEW_RA_POS:
                sprintf(buffer, "P%c%c%c%c%c%c%c", 2, 16, 36, rate, 0, 0, 0);
                break;
        case NEXSTAR_SLEW_RA_NEG:
                sprintf(buffer, "P%c%c%c%c%c%c%c", 2, 16, 37, rate, 0, 0, 0);
                break;
        case NEXSTAR_SLEW_DEC_POS:
                sprintf(buffer, "P%c%c%c%c%c%c%c", 2, 17, 36, rate, 0, 0, 0);
                break;
        case NEXSTAR_SLEW_DEC_NEG:
                sprintf(buffer, "P%c%c%c%c%c%c%c", 2, 17, 37, rate, 0, 0, 0);
                break;
        }

        serial_job(&nexstar->com, code, buffer);

        return 1;
}


int nexstar_update(struct nexstar_t *nexstar)
{
        static bool first = true;
        static char buf1[256];
        static char buf2[256];

        struct serial_job *job;

        if (first == true) {
                serial_job(&nexstar->com, NEXSTAR_RADEC, "e");
                serial_job(&nexstar->com, NEXSTAR_ALTAZ, "z");
                first = false;
                return 0;
        }

        if ((job = serial_run_jobs(&nexstar->com))) {

                switch (job->code) {
                case NEXSTAR_SLEW_RA_POS:
                        printf("slewing\n");
                        free(job);
                        /*serial_job(&nexstar->com, &radec, "e");*/
                        return 1;

                case NEXSTAR_RADEC:
                        memset(buf1, 0, 256);
                        memset(buf2, 0, 256);
                        parse_2x8(job->recv, buf1, buf2);

                        nexstar->ra.raw = (int)strtol(buf1, NULL, 16);
                        nexstar->ra.deg = NEXSTAR_32BIT_TO_DEGREE(nexstar->ra.raw);
                        nexstar->ra.arc = NEXSTAR_32BIT_TO_ARCSEC(nexstar->ra.raw);

                        nexstar->dec.raw = (int)strtol(buf2, NULL, 16);
                        nexstar->dec.deg = NEXSTAR_32BIT_TO_DEGREE(nexstar->dec.raw);
                        nexstar->dec.arc = NEXSTAR_32BIT_TO_ARCSEC(nexstar->dec.raw);

                        /* Re-issue */
                        serial_job(&nexstar->com, NEXSTAR_RADEC, "e");

                        return 1;

                case NEXSTAR_ALTAZ:
                        memset(buf1, 0, 256);
                        memset(buf2, 0, 256);
                        parse_2x8(job->recv, buf1, buf2);

                        nexstar->az.raw = (int)strtol(buf1, NULL, 16);
                        nexstar->az.deg = NEXSTAR_32BIT_TO_DEGREE(nexstar->az.raw);
                        nexstar->az.arc = NEXSTAR_32BIT_TO_ARCSEC(nexstar->az.raw);

                        nexstar->alt.raw = (int)strtol(buf2, NULL, 16);
                        nexstar->alt.deg = NEXSTAR_32BIT_TO_DEGREE(nexstar->alt.raw);
                        nexstar->alt.arc = NEXSTAR_32BIT_TO_ARCSEC(nexstar->alt.raw);

                        /* Re-issue */
                        serial_job(&nexstar->com, NEXSTAR_ALTAZ, "e");

                        return 1;
                default:
                        return 1;
                }
        }

        return 0;
}


