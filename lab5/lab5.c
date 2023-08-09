// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include <lcom/timer.h>
#include "keybord.h"
#include "video_gr.h"

#include <stdint.h>
#include <stdio.h>

extern int scancode;
extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (vg_init(mode) == NULL) return 1;
  sleep(delay);
  if (vg_exit()) return 1;
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  uint8_t bit_no;
  int ipc_status, i = 0, r;
  message msg;
  uint8_t sc[2] = {0, 0};

  if (kb_subscribe_int(&bit_no)) return 1;
  if (vg_init(mode) == NULL) return 1;
  
  if (vg_draw_rectangle(x, y, width, height, color)) return 1;

  while (scancode != ESC_BREAK) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:				
          if (msg.m_notify.interrupts & BIT(bit_no)) {
            kbc_ih();
            sc[i] = scancode;
            if (i == 0) {
              if (sc[i] == TWO_BYTE_SC) i = 1;
            } else {
              i = 0;
            }
          }
          break;
        default:
          break;	
      }
    }
  }

  if (kb_unsubscribe_int()) return 1;
  if (vg_exit()) return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  uint8_t bit_no;
  int ipc_status, i = 0, r;
  message msg;
  uint8_t sc[2] = {0, 0};

  if (kb_subscribe_int(&bit_no)) return 1;
  if (vg_init(mode) == NULL) return 1;
  
  if (vg_draw_pattern(no_rectangles, first, step)) return 1;

  while (scancode != ESC_BREAK) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:				
          if (msg.m_notify.interrupts & BIT(bit_no)) {
            kbc_ih();
            sc[i] = scancode;
            if (i == 0) {
              if (sc[i] == TWO_BYTE_SC) i = 1;
            } else {
              i = 0;
            }
          }
          break;
        default:
          break;	
      }
    }
  }

  if (kb_unsubscribe_int()) return 1;
  if (vg_exit()) return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  uint8_t bit_no;
  int ipc_status, i = 0, r;
  message msg;
  uint8_t sc[2] = {0, 0};

  if (kb_subscribe_int(&bit_no)) return 1;
  if (vg_init(INDEXED_MODE) == NULL) return 1;
  
  xpm_image_t img_info;

  if (xpm_load(xpm, XPM_INDEXED, &img_info) == NULL) return 1;

  if (vg_draw_xpm(img_info, x, y)) return 1;

  while (scancode != ESC_BREAK) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:				
          if (msg.m_notify.interrupts & BIT(bit_no)) {
            kbc_ih();
            sc[i] = scancode;
            if (i == 0) {
              if (sc[i] == TWO_BYTE_SC) i = 1;
            } else {
              i = 0;
            }
          }
          break;
        default:
          break;	
      }
    }
  }

  if (kb_unsubscribe_int()) return 1;
  if (vg_exit()) return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  uint8_t bit_no, bit_no2;
  int ipc_status, i = 0, r;
  message msg;
  int16_t toIgnore;
  uint8_t sc[2] = {0, 0};
  bool horizontal_move = (yi == yf);

  if (kb_subscribe_int(&bit_no)) return 1;
  if (timer_subscribe_int(&bit_no2)) return 1;
  if (vg_init(INDEXED_MODE) == NULL) return 1;
  
  xpm_image_t img_info;
  
  if (speed < 0) toIgnore = -speed - 1;
  else toIgnore = 0;

  if (xpm_load(xpm, XPM_INDEXED, &img_info) == NULL) return 1;

  if (vg_draw_xpm(img_info, xi, yi)) return 1;


  while (scancode != ESC_BREAK) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:				
          if (msg.m_notify.interrupts & BIT(bit_no)) {
            kbc_ih();
            sc[i] = scancode;
            if (i == 0) {
              if (sc[i] == TWO_BYTE_SC) i = 1;
            } else {
              i = 0;
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no2)) {
            timer_int_handler();
            if (timer_counter % (sys_hz()/fr_rate) == 0) {
              timer_counter = 0;
              if (horizontal_move && (xi < xf)) {
                if (toIgnore == 0) {
                  vg_erase_xpm(img_info, xi, yi);
                  if (speed > 0) {
                    xi += speed;
                  }
                  else {
                    xi++;
                    toIgnore = - speed - 1;
                  }
                  if (xi > xf) xi = xf;
                  vg_draw_xpm(img_info, xi, yi);
                } else if (speed < 0) toIgnore--;
              } else if (!horizontal_move && (yi < yf)) {
                if (toIgnore == 0) {
                  vg_erase_xpm(img_info, xi, yi);
                  if (speed > 0) {
                    yi += speed;
                  }
                  else {
                    yi++;
                    toIgnore = - speed - 1;
                  }
                  if (yi > yf) yi = yf;
                  vg_draw_xpm(img_info, xi, yi);
                } else if (speed < 0) toIgnore--;
              }
            }
          }
          break;
        default:
          break;	
      }
    }
  }

  if (kb_unsubscribe_int()) return 1;
  if (timer_unsubscribe_int()) return 1;
  if (vg_exit()) return 1;

  return 0;
}
