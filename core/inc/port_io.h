#pragma once

#include "common.h"

/**
 * @brief Write a byte to a port
 */
extern void wb_port(u16 port, u8 wbyte);

/**
 * @brief Read a byte from a port
 */
extern u8 rb_port(u16 port);