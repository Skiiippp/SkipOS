#pragma once

#include "../inc/common.h"

#include <stdint-gcc.h>

void SER_init();

void SER_write(const u8 *buff, size_t len);

void SER_flush();