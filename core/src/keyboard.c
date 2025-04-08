/**
 * NOTE: Keyboard in QEMU is on PS/2 controller's first port
 * NOTE: Use scan code 2
 */

#include "../inc/keyboard.h"

#include "../inc/port_io.h"
#include "../inc/common.h"
#include "../inc/assert.h"
#include "../inc/printk.h"
#include "../inc/scan_code.h"

/**
 * BEGIN PRIVATE
 */

#define MAX_KBD_RESEND 3

#define IO_P_1 0x60 // Data
#define IO_P_2 0x64 // Status/Command

#define DISABLE_P1_CMD 0xAD
#define DISABLE_P2_CMD 0xA7
#define ENABLE_P1_CMD 0xAE
#define READ_CFG_CMD 0x20
#define WRITE_CFG_CMD 0x60
#define CNTRLR_SELF_TEST_CMD 0xAA

#define CNTRLR_SELF_TEST_PASS 0x55
#define CNTRLR_SELF_TEST_FAIL 0xFC

#define OUTPUT_BUFF_STATUS_MSK 0x1
#define INPUT_BUFF_STATUS_MSK (0x1 << 1)

#define P1_INT_CFG_MSK 0x1
#define P2_INT_CFG_MSK (0x1 << 1)
#define P1_CLK_CFG_MSK (0x1 << 4)
#define P2_CLK_CFG_MSK (0x1 << 5)
#define P1_TRANSLATION_CFG_MSK (0x1 << 6)

#define KBD_ACK_RSP 0xFA
#define KBD_RESEND_RSP 0xFE
#define KBD_SELF_TEST_PASS 0xAA

#define KBD_RST_CMD 0xFF
#define KBD_ECHO_CMD 0xEE
#define KBD_SET_SCAN_CODE_CMD 0xF0
#define KBD_SET_SCAN_CODE_2_DATA 2
#define KBD_ENABLE_SCAN_CMD 0xF4

#define L_SHIFT_SCODE 0x12
#define R_SHIFT_SCODE 0x59
#define RELEASED_SCODE 0xF0

static void disable_p1();

static void disable_p2();

static void enable_p1();

static u8 read_status_byte();

static void wait_read_data();

static void wait_write_data();

static u8 read_data_byte();

static void write_data_byte(u8 data_byte);

static void write_command_byte(u8 command_byte);

static void write_two_command_bytes(u8 fst_cmd_byte, u8 sec_cmd_byte);

static u8 read_config_byte();

static void write_config_byte(u8 config_byte);

static u8 get_good_config_byte(u8 curr_config_byte);

static void write_good_config_byte();

static void run_controller_self_test();

// NOTE: Verifies ACK internally!
static void send_kbd_byte(u8 kbd_byte);

static void verify_kbd_ack(u8 kbd_sent_byte);

static u8 recv_kbd_byte();

static void reset_kbd();

static void set_kbd_scan_code_2();

static void enable_scanning();

/**
 * END PRIVATE
 */

void KBD_init()
{
    disable_p1();

    disable_p2();

    write_good_config_byte();

    run_controller_self_test();

    enable_p1();

    reset_kbd();

    set_kbd_scan_code_2();
}

void KBD_run()
{
    u8 s;
    char c;
    bool shift_pressed = false;

    while(1)
    {
        // Released?
        if ((s = recv_kbd_byte()) == RELEASED_SCODE)
        {
            if ((s = recv_kbd_byte()) == L_SHIFT_SCODE || s == R_SHIFT_SCODE)
            {
                shift_pressed = false;
            }
        }
        else if (s == L_SHIFT_SCODE || s == R_SHIFT_SCODE)
        {
            shift_pressed = true;
        }
        else
        {
            c = char_from_scode(s, shift_pressed);
            printk("%c", c);
        }
    }
}

void disable_p1()
{
    write_command_byte(DISABLE_P1_CMD);
}

void disable_p2()
{
    write_command_byte(DISABLE_P2_CMD);
}

void enable_p1()
{
    write_command_byte(ENABLE_P1_CMD);
}

u8 read_status_byte()
{
    return rb_port(IO_P_2);
}

void wait_read_data()
{
    u8 status_byte = read_status_byte();

    // Wait for read buff to be full
    while(!(status_byte & OUTPUT_BUFF_STATUS_MSK))
    {
        status_byte = read_status_byte();
    }
}

void wait_write_data()
{
    u8 status_byte = read_status_byte();

    // Wait for write buff to be empty
    while(status_byte & INPUT_BUFF_STATUS_MSK)
    {
        status_byte = read_status_byte();
    }
}

u8 read_data_byte()
{
    wait_read_data();
    return rb_port(IO_P_1);
}

static void write_data_byte(u8 data_byte)
{
    wait_write_data();
    wb_port(IO_P_1, data_byte);
}

void write_command_byte(u8 command_byte)
{
    wb_port(IO_P_2, command_byte);
}

void write_two_command_bytes(u8 fst_cmd_byte, u8 sec_cmd_byte)
{
    write_command_byte(fst_cmd_byte);
    write_data_byte(sec_cmd_byte);
}

u8 read_config_byte()
{
    write_command_byte(READ_CFG_CMD);
    return read_data_byte();
}

void write_config_byte(u8 config_byte)
{
    write_two_command_bytes(WRITE_CFG_CMD, config_byte);
}

u8 get_good_config_byte(u8 curr_config_byte)
{
    curr_config_byte |= P1_INT_CFG_MSK;
    curr_config_byte &= ~P2_INT_CFG_MSK;

    curr_config_byte &= ~P1_CLK_CFG_MSK;
    curr_config_byte |=P2_CLK_CFG_MSK;

    curr_config_byte &= ~P1_TRANSLATION_CFG_MSK;

    return curr_config_byte;
}

void write_good_config_byte()
{
    write_config_byte(get_good_config_byte(read_config_byte()));
}

void run_controller_self_test()
{
    u8 self_test_resp;

    write_command_byte(CNTRLR_SELF_TEST_CMD);

    self_test_resp = read_data_byte();

    assert(self_test_resp != CNTRLR_SELF_TEST_FAIL);
    assert(self_test_resp == CNTRLR_SELF_TEST_PASS);
}

void send_kbd_byte(u8 kbd_byte)
{
    write_data_byte(kbd_byte);
    verify_kbd_ack(kbd_byte);
}

static void verify_kbd_ack(u8 kbd_sent_byte)
{
    u8 num_resend = 0;
    u8 ack_byte;

    while((ack_byte = recv_kbd_byte()) != KBD_ACK_RSP && num_resend < MAX_KBD_RESEND)
    {
        write_data_byte(kbd_sent_byte);
        num_resend++;
    }

    if (num_resend == MAX_KBD_RESEND)
    {
        assert(0);
    }
}

u8 recv_kbd_byte()
{
    return read_data_byte();
}

void reset_kbd()
{
    send_kbd_byte(KBD_RST_CMD);

    assert(recv_kbd_byte() == KBD_SELF_TEST_PASS);
}

void set_kbd_scan_code_2()
{
    send_kbd_byte(KBD_SET_SCAN_CODE_CMD);
    send_kbd_byte(KBD_SET_SCAN_CODE_2_DATA);
}

void enable_scanning()
{
    send_kbd_byte(KBD_ENABLE_SCAN_CMD);
}


