#ifndef _LCOM_MACRO_H_
#define _LCOM_MACRO_H_

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

/* Keybord Controller */
#define KBC_IN_BUF 0x60         // used for sending commands to the keyboard
#define KBC_OUT_BUF 0x60        // used for receiving scancodes
#define KBC_ST_REG 0x64         // used for reading the KBC state
#define KBC_CMD_REG 0x64

/* KBC commands */
#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60

/* Status Register */
#define KBC_PAR_TO_ERROR BIT(7) | BIT(6)   // Parity or Timeout error - invalid data
#define KBC_IBF BIT(1)                     // Input buffer full
#define KBC_OBF BIT(0)                     // Input buffer full

/* KBC Scancodes */
#define ESC_MAKE 0x01
#define ESC_BREAK 0x81

#define TWO_BYTE_SC 0xE0

/**@}*/

#endif /* _LCOM_MACRO_H */
