#pragma once

/**
 * @brief Control output to the VGA console
 */

extern void VGA_clear();

extern void VGA_display_char(char c);

extern void VGA_display_str(const char *s);
 