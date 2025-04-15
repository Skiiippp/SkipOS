#include "../inc/interrupt.h"

/**
 * BEGIN PRIVATE
 */

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

void remap_pics();
 
/**
 * END PRIVATE
 */

void IRQ_init()
{
    CLI;


}

void remap_pics()
{
    
}