#pragma once

#include <stdint.h>
#include "macro.h"
#include "utils.h"

int (kb_subscribe_int)(uint8_t *bit_no);

int (kb_unsubscribe_int)();

int (kbc_issue_cmd)(uint8_t cmd);

int (kbc_read)();

void (kbc_ih)();

int (kb_enable_int)();
