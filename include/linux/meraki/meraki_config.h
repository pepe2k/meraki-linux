/*
 *  Meraki board data config driver
 *
 *  Copyright (C) 2013 Cisco Systems, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 */

#ifndef MERAKI_CONFIG_H
#define MERAKI_CONFIG_H

struct meraki_config_platform_data {
    unsigned int i2c_adapter_id;
    unsigned short i2c_address;
};

enum {
    MERAKI_BOARD_UNKNOWN,
    MERAKI_BOARD_MS220_24,
    MERAKI_BOARD_MS220_24P,
    MERAKI_BOARD_MS220_48,
    MERAKI_BOARD_MS220_48LP,
    MERAKI_BOARD_MS220_48FP,
    MERAKI_BOARD_MS320_24,
    MERAKI_BOARD_MS320_24P,
    MERAKI_BOARD_MS320_48,
    MERAKI_BOARD_MS320_48LP,
    MERAKI_BOARD_MS320_48FP,
};

extern int meraki_config_get_product(void);

#endif