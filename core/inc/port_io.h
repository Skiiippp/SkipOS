#pragma once

#include "common.h"

/**
 * @brief Write a byte to a port
 */
void outb(u16 port, u8 val);

/**
 * @brief Read a byte from a port
 */
u8 inb(u16 port);

/**
 * @brief Wait for 1-4 microseconds
 */
void io_wait();