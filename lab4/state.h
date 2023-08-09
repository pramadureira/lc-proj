#ifndef _STATE_H_
#define _STATE_H_

#include "macro.h"

void (gesture_handler)(struct packet* pp, uint8_t x_len, uint8_t tolerance, gesture_state state);

int (change_state)(gesture_state* state);

#endif
