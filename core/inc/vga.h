#pragma once

/**
 * @brief Control output to the VGA console
 */

void VGA_clear();

void VGA_display_char(char c);

void VGA_display_str(const char *s);

// Only works per line
void VGA_backspace_char();
 