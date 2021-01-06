/*
 * Copyright © 2001-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MODBUS_RTU_PRIVATE_H
#define MODBUS_RTU_PRIVATE_H

#include <stdint.h>

#define _MODBUS_RTU_HEADER_LENGTH      1
#define _MODBUS_RTU_PRESET_REQ_LENGTH  6
#define _MODBUS_RTU_PRESET_RSP_LENGTH  2

#define _MODBUS_RTU_CHECKSUM_LENGTH    2

typedef struct _modbus_rtu {
    /* Device: "/dev/ttyS0", "/dev/ttyUSB0" or "/dev/tty.USA19*" on Mac OS X. */
    char *device;
    /* Bauds: 9600, 19200, 57600, 115200, etc */
    int baud;
    /* Data bit */
    uint8_t data_bit;
    /* Stop bit */
    uint8_t stop_bit;
    /* Parity: 'N', 'O', 'E' */
    char parity;

    /* To handle many slaves on the same link */
    int confirmation_to_ignore;
} modbus_rtu_t;

extern int libmodbuserrno;

/* Port functions */
ssize_t modbus_serial_send(const uint8_t *req, int req_length);
ssize_t modbus_serial_recv(uint8_t *rsp, int rsp_length, int response_timeout);
int modbus_serial_connect(const char *device, uint32_t baud, uint8_t parity, uint8_t data_bit, char stop_bit);

#endif /* MODBUS_RTU_PRIVATE_H */
