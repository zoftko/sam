// clang-format off
#include "bits.h"
#include "config.h"
// clang-format on

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "lcd.h"
#include "tx.h"

#define UP_CHAR_BTN PINC1
#define ERASE_CHAR_BTN PINC2
#define SET_CHAR_BTN PINC3
#define TX_BTN PINC4

#define AVAILABLE_CHAR_COUNT 10

const unsigned char DEFAULT_MESSAGE[] = "hello world!";
const unsigned char AVAILABLE_CHARS[AVAILABLE_CHAR_COUNT] = {' ', 'a', 'b', 'c', 'd',
                                                             'e', 'f', 'g', 'h', 'i'};

unsigned char text_buffer[32] = {0};
struct Frame frame;
struct Display display = {
    .rs_port = &PORTB,
    .rs_bit = PINB4,
    .en_port = &PORTD,
    .en_bit = PIND7,
    .data_port = &PORTB,
    .data_shift = 0};

unsigned char current_char = 0;

void next_char(void) {
    if (current_char == (AVAILABLE_CHAR_COUNT - 1)) {
        current_char = 0;
    } else {
        current_char++;
    }
}

void prev_char(void) {
    if (current_char == 0) {
        current_char = (AVAILABLE_CHAR_COUNT - 1);
    } else {
        current_char--;
    }
}

uint8_t is_pressed(volatile uint8_t *address, uint8_t pin) {
    if (read_bit(address, pin) == 0) {
        _delay_ms(DEBOUNCE_DELAY);
        if (read_bit(address, pin) == 0) {
            return 1;
        }
    }

    return 0;
}

void on_error(void) { TIMSK0 = 0x00; }

int main(void) {
    /**
     * 8-bit Timer0 setup
     * Generate interrupts at a 1kHz rate with a 64 bit pre-scaler.
     * Enable interrupts when the counter reaches the value of OCR0A.
     * */
    OCR0A = 64;
    TCCR0B = 0x03;
    TIMSK0 = 0x00;

    PORTB = 0x00;
    DDRB = 0xFF;

    PORTC = 0xFF;
    DDRC = 0x00;

    PORTD = 0x00;
    DDRD = 0xFF;

    tx_setup(&PORTD, PORTD5, &on_error);

    lcd_setup(&display);
    lcd_init(0x0D);
    lcd_clear_disp();

    sei();
    while (1) {
        if (is_pressed(&PINC, UP_CHAR_BTN)) {
            next_char();
            lcd_wr_char(AVAILABLE_CHARS[current_char]);
            lcd_step_cursor_left();
        }
        if (is_pressed(&PINC, ERASE_CHAR_BTN)) {
            lcd_step_cursor_left();
            lcd_wr_char(' ');
            lcd_step_cursor_left();
        }
        if (is_pressed(&PINC, SET_CHAR_BTN)) {
            lcd_step_cursor_right();
        }
    }
}

ISR(TIMER0_COMPA_vect) {}
