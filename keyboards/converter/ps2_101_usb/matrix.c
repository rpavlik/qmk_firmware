/*
Copyright 2011 Jun Wako <wakojun@gmail.com>
Copyright 2016 Ethan Apodaca <papodaca@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdbool.h>
#include "action.h"
#include "print.h"
#include "util.h"
#include "debug.h"
#include "ps2.h"
#include "host.h"
#include "led.h"
#include "matrix.h"

#define ROW_SHIFTER ((uint8_t)1)


static void matrix_make(uint8_t code);
static void matrix_break(uint8_t code);


/*
 * Matrix Array usage:
 * 'Scan Code Set 2' is assigned into 256(32x8)cell matrix.
 * Hmm, it is very sparse and not efficient :(
 *
 * Notes:
 * Both 'Hanguel/English'(F1) and 'Hanja'(F2) collide with 'Delete'(E0 71) and 'Down'(E0 72).
 * These two Korean keys need exceptional handling and are not supported for now. Sorry.
 *
 *    8bit wide
 *   +---------+
 *  0|         |
 *  :|   XX    | 00-7F for normal codes(without E0-prefix)
 *  f|_________|
 * 10|         |
 *  :|  E0 YY  | 80-FF for E0-prefixed codes
 * 1f|         |     (<YY>|0x80) is used as matrix position.
 *   +---------+
 *
 * Exceptions:
 * 0x83:    F7(0x83) This is a normal code but beyond  0x7F.
 * 0xFC:    PrintScreen
 * 0xFE:    Pause
 */
static uint8_t matrix[MATRIX_ROWS];

#define ROW(code) (code >> 3)
#define COL(code) (code & 0x07)

// matrix positions for exceptional keys
#define F7             (0x83)
#define PRINT_SCREEN   (0xFC)
#define PAUSE          (0xFE)

__attribute__ ((weak))
void matrix_init_kb(void) {
    matrix_init_user();
}

__attribute__ ((weak))
void matrix_scan_kb(void) {
    matrix_scan_user();
}

__attribute__ ((weak))
void matrix_init_user(void) {
}

__attribute__ ((weak))
void matrix_scan_user(void) { }

void matrix_init(void)
{
    debug_enable = true;
    //debug_matrix = true;
    //debug_keyboard = true;
    //debug_mouse = false;

    ps2_host_init();

    // initialize matrix state: all keys off
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0x00;
    }

    matrix_init_quantum();
}

/*
 * PS/2 Scan Code Set 2: Exceptional Handling
 *
 * There are several keys to be handled exceptionally.
 * The scan code for these keys are varied or prefix/postfix'd
 * depending on modifier key state.
 *
 * Keyboard Scan Code Specification:
 *     http://www.microsoft.com/whdc/archive/scancode.mspx
 *     http://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/scancode.doc
 *
 *
 * 1) Insert, Delete, Home, End, PageUp, PageDown, Up, Down, Right, Left
 *     a) when Num Lock is off
 *     modifiers | make                      | break
 *     ----------+---------------------------+----------------------
 *     Ohter     |                    <make> | <break>
 *     LShift    | E0 F0 12           <make> | <break>  E0 12
 *     RShift    | E0 F0 59           <make> | <break>  E0 59
 *     L+RShift  | E0 F0 12  E0 F0 59 <make> | <break>  E0 59 E0 12
 *
 *     b) when Num Lock is on
 *     modifiers | make                      | break
 *     ----------+---------------------------+----------------------
 *     Other     | E0 12              <make> | <break>  E0 F0 12
 *     Shift'd   |                    <make> | <break>
 *
 *     Handling: These prefix/postfix codes are ignored.
 *
 *
 * 2) Keypad /
 *     modifiers | make                      | break
 *     ----------+---------------------------+----------------------
 *     Ohter     |                    <make> | <break>
 *     LShift    | E0 F0 12           <make> | <break>  E0 12
 *     RShift    | E0 F0 59           <make> | <break>  E0 59
 *     L+RShift  | E0 F0 12  E0 F0 59 <make> | <break>  E0 59 E0 12
 *
 *     Handling: These prefix/postfix codes are ignored.
 *
 *
 * 3) PrintScreen
 *     modifiers | make         | break
 *     ----------+--------------+-----------------------------------
 *     Other     | E0 12  E0 7C | E0 F0 7C  E0 F0 12
 *     Shift'd   |        E0 7C | E0 F0 7C
 *     Control'd |        E0 7C | E0 F0 7C
 *     Alt'd     |           84 | F0 84
 *
 *     Handling: These prefix/postfix codes are ignored, and both scan codes
 *               'E0 7C' and 84 are seen as PrintScreen.
 *
 * 4) Pause
 *     modifiers | make(no break code)
 *     ----------+--------------------------------------------------
 *     Other     | E1 14 77 E1 F0 14 F0 77
 *     Control'd | E0 7E E0 F0 7E
 *
 *     Handling: Both code sequences are treated as a whole.
 *               And we need a ad hoc 'pseudo break code' hack to get the key off
 *               because it has no break code.
 *
 */
uint8_t matrix_scan(void)
{

    // scan code reading states
    static enum {
        INIT,
        F0,
        E0,
        E0_F0,
        // Pause
        E1,
        E1_14,
        E1_14_77,
        E1_14_77_E1,
        E1_14_77_E1_F0,
        E1_14_77_E1_F0_14,
        E1_14_77_E1_F0_14_F0,
        // Control'd Pause
        E0_7E,
        E0_7E_E0,
        E0_7E_E0_F0,
    } state = INIT;


    is_modified = false;

    // 'pseudo break code' hack
    if (matrix_is_on(ROW(PAUSE), COL(PAUSE))) {
       matrix_break(PAUSE);
    }

    uint8_t code;
    if ((code = ps2_host_recv())) {
        debug("r"); debug_hex(code); debug(" ");
    }
    if (!ps2_error) {
        switch (state) {
            case INIT:
                switch (code) {
                    case 0xE0:
                        state = E0;
                        break;
                    case 0xF0:
                        state = F0;
                        break;
                    case 0xE1:
                        state = E1;
                        break;
                    case 0x83:  // F7
                        matrix_make(F7);
                        state = INIT;
                        break;
                    case 0x84:  // Alt'd PrintScreen
                        matrix_make(PRINT_SCREEN);
                        state = INIT;
                        break;
                    case 0x00:  // Overrun [3]p.25
                        matrix_clear();
                        clear_keyboard();
                        debug("Overrun");
                        state = INIT;
                        break;
                    case 0xAA:  // Self-test passed
                    case 0xFC:  // Self-test failed
                        debug((code == 0xAA) ? "BAT OK" : "BAT NG");
                        led_set(host_keyboard_leds());
                        state = INIT;
                        break;
                    default:    // normal key make
                        if (code < 0x80) {
                            matrix_make(code);
                        } else {
                            matrix_clear();
                            clear_keyboard();
                            debug("unexpected scan code at INIT: "); debug_hex(code);
                        }
                        state = INIT;
                }
                break;
            case E0:    // E0-Prefixed
                switch (code) {
                    case 0x12:  // to be ignored
                    case 0x59:  // to be ignored
                        state = INIT;
                        break;
                    case 0x7E:  // Control'd Pause
                        state = E0_7E;
                        break;
                    case 0xF0:
                        state = E0_F0;
                        break;
                    default:
                        if (code < 0x80) {
                            matrix_make(code|0x80);
                        } else {
                            matrix_clear();
                            clear_keyboard();
                            debug("unexpected scan code at E0: "); debug_hex(code);
                        }
                        state = INIT;
                }
                break;
            case F0:    // Break code
                switch (code) {
                    case 0x83:  // F7
                        matrix_break(F7);
                        state = INIT;
                        break;
                    case 0x84:  // Alt'd PrintScreen
                        matrix_break(PRINT_SCREEN);
                        state = INIT;
                        break;
                    case 0xF0:
                        matrix_clear();
                        clear_keyboard();
                        debug("unexpected scan code at F0: F0(clear and cont.)");
                        break;
                    default:
                    if (code < 0x80) {
                        matrix_break(code);
                    } else {
                        matrix_clear();
                        clear_keyboard();
                        debug("unexpected scan code at F0: "); debug_hex(code);
                    }
                    state = INIT;
                }
                break;
            case E0_F0: // Break code of E0-prefixed
                switch (code) {
                    case 0x12:  // to be ignored
                    case 0x59:  // to be ignored
                        state = INIT;
                        break;
                    default:
                        if (code < 0x80) {
                            matrix_break(code|0x80);
                        } else {
                            matrix_clear();
                            clear_keyboard();
                            debug("unexpected scan code at E0_F0: "); debug_hex(code);
                        }
                        state = INIT;
                }
                break;
            // following are states of Pause
            case E1:
                switch (code) {
                    case 0x14:
                        state = E1_14;
                        break;
                    default:
                        state = INIT;
                }
                break;
            case E1_14:
                switch (code) {
                    case 0x77:
                        state = E1_14_77;
                        break;
                    default:
                        state = INIT;
                }
                break;
            case E1_14_77:
                switch (code) {
                    case 0xE1:
                        state = E1_14_77_E1;
                        break;
                    default:
                        state = INIT;
                }
                break;
            case E1_14_77_E1:
                switch (code) {
                    case 0xF0:
                        state = E1_14_77_E1_F0;
                        break;
                    default:
                        state = INIT;
                }
                break;
            case E1_14_77_E1_F0:
                switch (code) {
                    case 0x14:
                        state = E1_14_77_E1_F0_14;
                        break;
                    default:
                        state = INIT;
                }
                break;
            case E1_14_77_E1_F0_14:
                switch (code) {
                    case 0xF0:
                        state = E1_14_77_E1_F0_14_F0;
                        break;
                    default:
                        state = INIT;
                }
                break;
            case E1_14_77_E1_F0_14_F0:
                switch (code) {
                    case 0x77:
                        matrix_make(PAUSE);
                        state = INIT;
                        break;
                    default:
                        state = INIT;
                }
                break;
            // Following are states of Control'd Pause
            case E0_7E:
                if (code == 0xE0)
                    state = E0_7E_E0;
                else
                    state = INIT;
                break;
            case E0_7E_E0:
                if (code == 0xF0)
                    state = E0_7E_E0_F0;
                else
                    state = INIT;
                break;
            case E0_7E_E0_F0:
                if (code == 0x7E)
                    matrix_make(PAUSE);
                state = INIT;
                break;
            default:
                state = INIT;
        }
    }

    // TODO: request RESEND when error occurs?
/*
    if (PS2_IS_FAILED(ps2_error)) {
        uint8_t ret = ps2_host_send(PS2_RESEND);
        xprintf("Resend: %02X\n", ret);
    }
*/
    matrix_scan_quantum();

    return 1;
}

inline
uint8_t matrix_get_row(uint8_t row) {
    return matrix[row];
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop(matrix[i]);
    }
    return count;
}

inline static void matrix_make(uint8_t code) {
    if (!matrix_is_on(ROW(code), COL(code))) {
        matrix[ROW(code)] |= 1 << COL(code);
    }
}

inline static void matrix_break(uint8_t code) {
    if (matrix_is_on(ROW(code), COL(code))) {
        matrix[ROW(code)] &= ~(1 << COL(code));
    }
}

void matrix_clear(void) {
    for (uint8_t i=0; i < MATRIX_ROWS; i++) matrix[i] = 0x00;
}

bool matrix_is_on(uint8_t row, uint8_t col) {
    return (matrix_get_row(row) & (1 << col));
}

#if (MATRIX_COLS <= 8)
#    define print_matrix_header() print("\nr/c 01234567\n")
#    define print_matrix_row(row) print_bin_reverse8(matrix_get_row(row))
#elif (MATRIX_COLS <= 16)
#    define print_matrix_header() print("\nr/c 0123456789ABCDEF\n")
#    define print_matrix_row(row) print_bin_reverse16(matrix_get_row(row))
#elif (MATRIX_COLS <= 32)
#    define print_matrix_header() print("\nr/c 0123456789ABCDEF0123456789ABCDEF\n")
#    define print_matrix_row(row) print_bin_reverse32(matrix_get_row(row))
#endif

void matrix_print(void) {
    print_matrix_header();

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        print_hex8(row);
        print(": ");
        print_matrix_row(row);
        print("\n");
    }
}

