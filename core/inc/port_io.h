#pragma once

#include "common.h"

/**
 * @brief Write a byte to a port
 */
extern void outb(u16 port, u8 val);

/**
 * @brief Read a byte from a port
 */
extern u8 inb(u16 port);

/**
 * @brief Wait for 1-4 microseconds
 */
extern void io_wait();