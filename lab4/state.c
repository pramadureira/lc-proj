#include <lcom/lcf.h>

#include "state.h"
#include "macro.h"
#include "mouse.h"
#include "utils.h"

static gesture_movement move = OTHER;
static int8_t x = 0, y = 0;

void (gesture_handler)(struct packet* pp, uint8_t x_len, uint8_t tolerance, gesture_state state) {
    if (pp->lb && !pp->rb && !pp->mb) {
        if ((pp->delta_x >= 0 && pp->delta_y >= 0) || ((abs(pp->delta_x)) < tolerance && abs(pp->delta_y) < tolerance)) {
            x += pp->delta_x;
            y += pp->delta_y;
            move = FST_MOVE;
        } else {
            x = 0;
            y = 0;
            move = OTHER;
        }
    } else if (!pp->lb && pp->rb && !pp->mb) {
        if ((pp->delta_x >= 0 && pp->delta_y <= 0) || (abs(pp->delta_x) < tolerance && pp->delta_y < tolerance)) {
            x += pp->delta_x;
            y += pp->delta_y;
            move = SCD_MOVE;
        } else {
            x = 0;
            y = 0;
            move = OTHER;
        }
    } else if (!pp->lb && !pp->mb && !pp->mb) {
        if (abs(pp->delta_x) > tolerance || abs(pp->delta_y) > tolerance) {
            x = 0;
            y = 0;
            move = OTHER;
        } else if (x >= x_len && y > x) {
            x = 0;
            y = 0;
            move = FST_LINE;
        } else if (x >= x_len && y < -x) {
            x = 0;
            y = 0;
            move = SCD_LINE;
        } else {
            x = 0;
            y = 0;
            move = NO_MOVE;
        }
    } else {
        x = 0;
        y = 0;
        move = OTHER;
    }
}

int (change_state)(gesture_state* state) {
    switch (*state) {
    case INIT:
        if (move == FST_MOVE) *state = FIRST;
        break;
    case FIRST:
        if (move == FST_MOVE) break;
        else if (move == FST_LINE) *state = VERTEX;
        else *state = INIT;
        break;
    case VERTEX:
        if (move == SCD_MOVE) *state = SECOND;
        else if (move == NO_MOVE) break;
        else *state = INIT;
        break;
    case SECOND:
        if (move == SCD_MOVE) break;
        else if (move == SCD_LINE) *state = END;
        else *state = INIT;
        break;
    default:
        return 1;
    }
    return 0;
}
