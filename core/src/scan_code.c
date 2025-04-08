#include "../inc/scan_code.h"

/**
 * BEGIN PRIVATE
 */

typedef struct ScodeMapElem
{
    char c; // char
    char shift_c; // "capitalized" c
    u8 s;   // scode
} ScodeMapElem;


ScodeMapElem sc_map[] = {{'a', 'A', 0x1C}, {'b', 'B', 0x32}, {'c', 'C', 0x21}, {'d', 'D', 0x23},    \
    {'e', 'E', 0x24}, {'f', 'F', 0x2B}, {'g', 'G', 0x34}, {'h', 'H', 0x33}, {'i', 'I', 0x43}, {'j', 'J', 0x3B}, \
    {'k', 'K', 0x42}, {'l', 'L', 0x4B}, {'m', 'M', 0x3A}, {'n', 'N', 0x31}, {'o', 'O', 0x44}, {'p', 'P', 0x4D},   \
    {'q', 'Q', 0x15}, {'r', 'R', 0x2D}, {'s', 'S', 0x1B}, {'t', 'T', 0x2C}, {'u', 'U', 0x3C}, {'v', 'V', 0x2A},   \
    {'w', 'W', 0x1D}, {'x', 'X', 0x22}, {'y', 'Y', 0x35}, {'z', 'Z', 0x1A}, {'1', '!', 0x16}, {'2', '@', 0x1E}, \
    {'3', '#', 0x26}, {'4', '$', 0x25}, {'5', '%', 0x2E}, {'6', '^', 0x36}, {'7', '&', 0x3D}, {'8', '*', 0x3E}, \
    {'9', '(', 0x46}, {'0', ')', 0x45}, {'\n', '\n', 0x5A}, {'\t', '\t', 0x0D}, {',', '<', 0x41}, {'.', '>', 0x49}, \
    {'/', '?', 0x4A}, {' ', ' ', 0x29}};          

/**
 * END PRIVATE
 */

// TODO: Use lookup table instead of looping
char char_from_scode(u8 scode, bool shift_pressed)
{
    size_t map_size = sizeof(sc_map);

    for (size_t i = 0; i < map_size; i++)
    {
        if(sc_map[i].s == scode)
        {
            if(shift_pressed)
            {
                return sc_map[i].shift_c;
            }
            return sc_map[i].c;
        }
    }

    // Not found!
    return ' ';
}