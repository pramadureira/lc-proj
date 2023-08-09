#pragma once

#include <stdint.h>

int (vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step);

uint8_t clear_bits(uint8_t color, uint8_t starting_pos);

int (vg_draw_xpm)(xpm_image_t img_info, uint16_t x, uint16_t y);

int (vg_erase_xpm)(xpm_image_t img_info, uint16_t x, uint16_t y);
