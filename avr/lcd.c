// clang-format off
#include "config.h"
// clang-format on

#include "lcd.h"

#include <stdint.h>
#include <util/delay.h>

#include "bits.h"

static const struct Display *lcd_conf;
static uint8_t cursor_position;

void toggle_enable(void) {
    bit_on(lcd_conf->en_port, lcd_conf->en_bit);
    _delay_ms(2);
    bit_off(lcd_conf->en_port, lcd_conf->en_bit);
}

void lcd_setup(const struct Display *display) {
    lcd_conf = display;
    cursor_position = 0;
    lcd_init(lcd_conf->config);
}

void lcd_send(uint8_t rs, uint8_t value) {
    if (rs == 0) {
        bit_off(lcd_conf->rs_port, lcd_conf->rs_bit);
    } else {
        bit_on(lcd_conf->rs_port, lcd_conf->rs_bit);
    }

    set_high_nibble(lcd_conf->data_port, value, lcd_conf->data_shift);
    toggle_enable();

    set_low_nibble(lcd_conf->data_port, value, lcd_conf->data_shift);
    toggle_enable();
}

void lcd_init(uint8_t config) {
    lcd_send(0, 0x02);
    lcd_send(0, 0x28);
    lcd_send(0, config);
    lcd_send(0, 0x14);
    lcd_send(0, 0x01);
}

void lcd_wr_str(const unsigned char *str) {
    while (*str != '\0') {
        lcd_send(1, *str);
        str++;
    }
}

void lcd_wr_char(unsigned char value) { lcd_send(1, value); }

void lcd_mv_cursor(uint8_t address) { lcd_send(0, (0x80 | address)); }

void lcd_step_cursor_right(void) { lcd_send(0, 0x14); }

void lcd_step_cursor_left(void) { lcd_send(0, 0x10); }

void lcd_clear_disp(void) { lcd_send(0, 0x01); }
