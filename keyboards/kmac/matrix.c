/*
Copyright 2017 Mathias Andersson <wraul@dbox.se>

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
#if defined(__AVR__)
#include <avr/io.h>
#endif
#include "wait.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "timer.h"


/* Set 0 if debouncing isn't needed */
#ifndef DEBOUNCE
#   define DEBOUNCE 5
#endif

#define COL_SHIFTER ((uint32_t)1)

static uint16_t debouncing_time;
static bool debouncing = false;


static const uint8_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static void init_rows(void);
static bool read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col);
static void unselect_cols(void);
static void select_col(uint8_t col);

inline
uint8_t matrix_rows(void) {
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void) {
    return MATRIX_COLS;
}

void matrix_init(void) {
    unselect_cols();
    init_rows();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }

    matrix_init_quantum();
}

uint8_t matrix_scan(void)
{
    // Set col, read rows
    for (uint8_t current_col = 0; current_col < MATRIX_COLS; current_col++) {
        bool matrix_changed = read_rows_on_col(matrix_debouncing, current_col);
        if (matrix_changed) {
            debouncing = true;
            debouncing_time = timer_read();
        }
    }

    if (debouncing && (timer_elapsed(debouncing_time) > DEBOUNCE)) {
        for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
            matrix[i] = matrix_debouncing[i];
        }
        debouncing = false;
    }

    matrix_scan_quantum();
    return 1;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEFGHIJKLMNOPQRSTUV\n");

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        print_bin_reverse32(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop32(matrix[i]);
    }
    return count;
}

static bool read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col)
{
    bool matrix_changed = false;

    // Select col and wait for col selecton to stabilize
    select_col(current_col);
    wait_us(30);

    // For each row...
    for(uint8_t row_index = 0; row_index < MATRIX_ROWS; row_index++)
    {
        // Store last value of row prior to reading
        matrix_row_t last_row_value = current_matrix[row_index];

        // Check row pin state
        // Use the otherwise unused row: 3, col: 0 for caps lock
        if (row_index == 3 && current_col == 0) {
            // Pin E2 uses active low
            if ((_SFR_IO8(E2 >> 4) & _BV(E2 & 0xF)) == 0)
            {
                // Pin LO, set col bit
                current_matrix[row_index] |= (COL_SHIFTER << current_col);
            }
            else
            {
                // Pin HI, clear col bit
                current_matrix[row_index] &= ~(COL_SHIFTER << current_col);
            }
        }
        else {
            if ((_SFR_IO8(row_pins[row_index] >> 4) & _BV(row_pins[row_index] & 0xF)))
            {
                // Pin HI, set col bit
                current_matrix[row_index] |= (COL_SHIFTER << current_col);
            }
            else
            {
                // Pin LO, clear col bit
                current_matrix[row_index] &= ~(COL_SHIFTER << current_col);
            }
        }

        // Determine if the matrix changed state
        if ((last_row_value != current_matrix[row_index]) && !(matrix_changed))
        {
            matrix_changed = true;
        }
    }

    // Unselect cols
    unselect_cols();

    return matrix_changed;
}

/* Row pin configuration
 * row: 0   1   2   3   4   5
 * pin: D0  D1  D2  D3  D5  B7
 *
 * Caps lock uses its own pin E2
 */
static void init_rows(void)
{
    DDRD  &= ~((1<<0)| (1<<1) | (1<<2) | (1<<3) | (1<<5)); // IN
    PORTD &= ~((1<<0)| (1<<1) | (1<<2) | (1<<3) | (1<<5)); // LO
    DDRB  &= ~(1<<7); // IN
    PORTB &= ~(1<<7); // LO

    DDRE &= ~(1<<2); // IN
    PORTE |= (1<<2); // HI
}

/* Columns 0 - 15
 * These columns uses two 74HC237D 3 to 8 bit demultiplexers.
 * col / pin:    PC6  PB6  PF0  PF1  PC7
 * 0:             1    0    0    0    0
 * 1:             1    0    1    0    0
 * 2:             1    0    0    1    0
 * 3:             1    0    1    1    0
 * 4:             1    0    0    0    1
 * 5:             1    0    1    0    1
 * 6:             1    0    0    1    1
 * 7:             1    0    1    1    1
 * 8:             0    1    0    0    0
 * 9:             0    1    1    0    0
 * 10:            0    1    0    1    0
 * 11:            0    1    1    1    0
 * 12:            0    1    0    0    1
 * 13:            0    1    1    0    1
 * 14:            0    1    0    1    1
 * 15:            0    1    1    1    1
 *
 * col: 16
 * pin: PB5
 */
static void unselect_cols(void)
{
    DDRB  |= (1<<5) | (1<<6); // OUT
    PORTB &= ~((1<<5) | (1<<6)); // LO

    DDRC  |= (1<<6) | (1<<7); // OUT
    PORTC &= ~((1<<6) | (1<<7)); // LO

    DDRF  |= (1<<0) | (1<<1); // OUT
    PORTF &= ~((1<<0) | (1<<1)); // LO
}

static void select_col(uint8_t col)
{
    switch (col) {
        case 0:
            PORTC |= (1<<6); // HI
            break;
        case 1:
            PORTC |= (1<<6); // HI
            PORTF |= (1<<0); // HI
            break;
        case 2:
            PORTC |= (1<<6); // HI
            PORTF |= (1<<1); // HI
            break;
        case 3:
            PORTC |= (1<<6); // HI
            PORTF |= (1<<0) | (1<<1); // HI
            break;
        case 4:
            PORTC |= (1<<6); // HI
            PORTC |= (1<<7); // HI
            break;
        case 5:
            PORTC |= (1<<6); // HI
            PORTF |= (1<<0); // HI
            PORTC |= (1<<7); // HI
            break;
        case 6:
            PORTC |= (1<<6); // HI
            PORTF |= (1<<1); // HI
            PORTC |= (1<<7); // HI
            break;
        case 7:
            PORTC |= (1<<6); // HI
            PORTF |= (1<<0) | (1<<1); // HI
            PORTC |= (1<<7); // HI
            break;
        case 8:
            PORTB |= (1<<6); // HI
            break;
        case 9:
            PORTB |= (1<<6); // HI
            PORTF |= (1<<0); // HI
            break;
        case 10:
            PORTB |= (1<<6); // HI
            PORTF |= (1<<1); // HI
            break;
        case 11:
            PORTB |= (1<<6); // HI
            PORTF |= (1<<0) | (1<<1); // HI
            break;
        case 12:
            PORTB |= (1<<6); // HI
            PORTC |= (1<<7); // HI
            break;
        case 13:
            PORTB |= (1<<6); // HI
            PORTF |= (1<<0); // HI
            PORTC |= (1<<7); // HI
            break;
        case 14:
            PORTB |= (1<<6); // HI
            PORTF |= (1<<1); // HI
            PORTC |= (1<<7); // HI
            break;
        case 15:
            PORTB |= (1<<6); // HI
            PORTF |= (1<<0) | (1<<1); // HI
            PORTC |= (1<<7); // HI
            break;
        case 16:
            PORTB |= (1<<5); // HI
            break;
    }
}
