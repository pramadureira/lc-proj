#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdint.h>

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

void (mouse_ih)();

int (mouse_issue_write_cmd)(uint8_t reg, uint8_t cmd);

int (mouse_read_data)(uint8_t *byte);

int (mouse_data_reporting)(uint8_t cmd);

struct packet (mouse_parse_packet)(uint8_t packet[3]);

#endif
