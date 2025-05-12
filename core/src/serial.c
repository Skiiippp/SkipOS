#include "../inc/serial.h"

#include "../inc/port_io.h"
#include "../inc/common.h"
#include "../inc/interrupt.h"
#include "../inc/assert.h"

/**
 * BEGIN PRIVATE
 */

#define COM1_IOP 0x3F8
#define COM1_IRQ_NUM 0x24

#define LCR_OFFSET 3
#define LSR_OFFSET 5
#define TRAN_OFFSET 0 // Write
#define INT_EN_REG_OFFSET 1
#define LSBY_DIV_OFFSET 0 // DLAB must be set
#define MSBY_DIV_OFFSET 1 // DLAB must be set
#define IIR_OFFSET 2 // Read
#define FIFO_CNTR_REG_OFFSET 2 // Write

#define UART_BAUD 9600
#define BASE_BAUD 115200
#define BAUD_DIVISOR (BASE_BAUD/UART_BAUD) // 115200/9600

#define BUFF_SIZE 64

typedef struct
{
    u8 buff[BUFF_SIZE];
    u8 *c, *p;
    bool busy;
} ser_state_t;

static ser_state_t ser_state;

static void init_hw_write(ser_state_t *s_ptr);

static void write_ser_reg(u8 offset, u8 val);

static u8 read_ser_reg(u8 offset);

static bool is_buff_empty(ser_state_t *s_ptr);

static bool is_buff_full(ser_state_t *s_ptr);

// Return true if consumed, false if empty
static bool consume(ser_state_t *s_ptr, u8 *c_val_ptr);

// Return true if produced, false if full
static bool produce(ser_state_t *s_ptr, u8 p_val);

static void ser_irq_handler(u8 irq_index, u32 error, void *arg);

/**
 * END PRIVATE
 */

void SER_init()
{
    write_ser_reg(LCR_OFFSET, 0b01000000);  // Set DLEB
    write_ser_reg(LSBY_DIV_OFFSET, BAUD_DIVISOR & 0xFF);    // Set baud
    write_ser_reg(MSBY_DIV_OFFSET, BAUD_DIVISOR >> 8);
    write_ser_reg(LCR_OFFSET, 0b00000011);  // Disable brk, 8N1
    write_ser_reg(INT_EN_REG_OFFSET, 0b00000010);   // Tran buff empty ints
    write_ser_reg(FIFO_CNTR_REG_OFFSET, 0); // Disable FIFOs, don't worry about recv

    ser_state.c = &ser_state.buff[0];
    ser_state.p = &ser_state.buff[0];
    ser_state.busy = false;

    IRQ_set_handler(COM1_IRQ_NUM, ser_irq_handler, &ser_state);
    IRQ_enable_index(COM1_IRQ_NUM);
}

void SER_write(const u8 *buff, size_t len)
{
    bool enable_ints = false;
    if (IRQ_are_interrupts_enabled())
    {
        enable_ints = true;
        CLI;
    }

    size_t i = 0;
    while(i < len && produce(&ser_state, buff[i]))
    {
        i++;
    }

    // start output
    init_hw_write(&ser_state);

    if (enable_ints)
    {
        STI;
    }
}

void init_hw_write(ser_state_t *s_ptr)
{
    assert(s_ptr);

    if (s_ptr->busy)
    {
        const u8 lsr = read_ser_reg(LSR_OFFSET);
        // Check if tx buff empty
        if (lsr & (1 << 5))
        {
            s_ptr->busy = false;
        }
    }

    u8 out_val;
    if (!s_ptr->busy)
    {
        if (consume(s_ptr, &out_val))
        {
            s_ptr->busy = true;
            write_ser_reg(TRAN_OFFSET, out_val);
        }
    }
}

void write_ser_reg(u8 offset, u8 val)
{
    outb(COM1_IOP + offset, val);
}

u8 read_ser_reg(u8 offset)
{
    return inb(COM1_IOP + offset);
}

bool is_buff_empty(ser_state_t *s_ptr)
{
    assert(!IRQ_are_interrupts_enabled());

    return (s_ptr->c == s_ptr->p);
}

bool is_buff_full(ser_state_t *s_ptr)
{
    assert(!IRQ_are_interrupts_enabled());

    return ((s_ptr->p == s_ptr->c - 1) || (s_ptr->p == s_ptr->c + BUFF_SIZE - 1));
}

bool consume(ser_state_t *s_ptr, u8 *c_val_ptr)
{
    assert(!IRQ_are_interrupts_enabled());

    if (is_buff_empty(s_ptr))
    {
        return false;
    }

    *c_val_ptr = *s_ptr->c;
    s_ptr->c++;
    if(s_ptr->c == &s_ptr->buff[BUFF_SIZE])
    {
        s_ptr->c = &s_ptr->buff[0];
    }

    return true;
}

bool produce(ser_state_t *s_ptr, u8 p_val)
{
    assert(!IRQ_are_interrupts_enabled());

    if (is_buff_full(s_ptr))
    {
        return false;
    }

    *s_ptr->p = p_val;
    s_ptr->p++;
    if(s_ptr->p == &s_ptr->buff[BUFF_SIZE])
    {
        s_ptr->p = &s_ptr->buff[0];
    }

    return true;
}

void ser_irq_handler(u8 irq_index, u32 error, void *arg)
{
    (void)error;

    assert(irq_index == COM1_IRQ_NUM);
    assert(arg);

    ser_state_t *s_ptr = (ser_state_t *)arg;
    const u8 iir_val = read_ser_reg(IIR_OFFSET);
    const u8 int_state = (iir_val >> 1) & 0xFF;

    if (int_state == 0b11)
    {
        // LSR info - read to clear
        read_ser_reg(LSR_OFFSET);
    }
    else if (int_state == 0b01)
    {
        // TX avail
        s_ptr->busy = false;
        init_hw_write(s_ptr);
    }

    IRQ_end_of_interrupt(irq_index);
}
