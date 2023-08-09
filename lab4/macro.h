#ifndef _LCOM_MACRO2_H_
#define _LCOM_MACRO2_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keybord.
 */

#define DELAY_US 20000

/* IRQ */
#define TIMER0_IRQ 0
#define KB_IRQ 1
#define MS_IRQ 12

/* Keybord Controller */
#define KBC_IN_BUF 0x60         // used for sending commands to the keyboard/mouse
#define KBC_OUT_BUF 0x60        // used for receiving scancodes
#define KBC_ST_REG 0x64         // used for reading the KBC state
#define KBC_CMD_REG 0x64

/* Mouse Controller */
#define MS_ACK 0xFA
#define MS_NACK 0xFE
#define MS_ERROR 0xFC

#define MS_LB BIT(0)
#define MS_RB BIT(1)
#define MS_MB BIT(2)
#define MS_X_MSB BIT(4)
#define MS_Y_MSB BIT(5)
#define MS_X_OVF BIT(6)
#define MS_Y_OVF BIT(7)

/* KBC commands */
#define KB_READ_CMD_BYTE 0x20
#define KB_WRITE_CMD_BYTE 0x60

#define MS_READ_CMD_BYTE 0x20
#define MS_WRITE_CMD_BYTE 0xD4
#define MS_ENABLE_REP 0xF4
#define MS_DISABLE_REP 0xF5

#define MS_FIRST_BYTE BIT(3)

/* Status Register */
#define KBC_PAR_TO_ERROR BIT(7) | BIT(6)   // Parity or Timeout error - invalid data
#define KBC_IBF BIT(1)                     // Input buffer full
#define KBC_OBF BIT(0)                     // Output buffer full
#define KBC_AUX BIT(5)                     // Mouse data

/* KBC Scancodes */
#define ESC_MAKE 0x01
#define ESC_BREAK 0x81

#define TWO_BYTE_SC 0xE0

typedef enum {
    INIT,
    FIRST,
    VERTEX,
    SECOND,
    END
} gesture_state;

typedef enum {
    FST_MOVE, FST_LINE, SCD_LINE, NO_MOVE, SCD_MOVE, OTHER
} gesture_movement;

/**@}*/

#endif /* _LCOM_MACRO2_H */
