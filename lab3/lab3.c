#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <lcom/timer.h>

#include "keybord.h"

#include <stdbool.h>
#include <stdint.h>

extern uint8_t scancode;
extern uint8_t status;
extern int ret_val;
extern int cnt;

extern int timer_counter;


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  int ipc_status, i = 0, r;
  message msg;
  uint8_t sc[2] = {0, 0};

  if (kb_subscribe_int(&bit_no)) return 1;

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
              else kbd_print_scancode(!(scancode & BIT(7)), 1, sc);
            } else {
              i = 0;
              kbd_print_scancode(!(scancode & BIT(7)), 2, sc);
            }
          }
          break;
        default:
          break;	
      }
    }
  }

  if (kb_unsubscribe_int()) return 1;

  if (kbd_print_no_sysinb(cnt)) return 1;

  return 0;
}

int(kbd_test_poll)() {
  int i = 0;
  uint8_t sc[2] = {0, 0};
  while (scancode != ESC_BREAK) {
    if (kbc_read()) continue;
    sc[i] = scancode;
    if (i == 0) {
      if (sc[i] == TWO_BYTE_SC) i = 1;
      else kbd_print_scancode(!(scancode & BIT(7)), 1, sc);
    } else {
      i = 0;
      kbd_print_scancode(!(scancode & BIT(7)), 2, sc);
    }
  }

  if (kb_enable_int()) return 1;

  if (kbd_print_no_sysinb(cnt)) return 1;

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no1, bit_no2, tc = n;
  int ipc_status, i = 0, r;
  message msg;
  uint8_t sc[2] = {0, 0};


  if (kb_subscribe_int(&bit_no1)) return 1;
  if (timer_subscribe_int(&bit_no2)) return 1;

  while (scancode != ESC_BREAK && tc > 0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:		
        if (msg.m_notify.interrupts & BIT(bit_no2)) {
            timer_int_handler();
            if (timer_counter % sys_hz() == 0) {
              tc--;
              timer_counter = 0;
              timer_print_elapsed_time();
            }
          }		
          if (msg.m_notify.interrupts & BIT(bit_no1)) {
            tc = n;
            timer_counter = 0;
            kbc_ih();
            sc[i] = scancode;
            if (i == 0) {
              if (sc[i] == TWO_BYTE_SC) i = 1;
              else kbd_print_scancode(!(scancode & BIT(7)), 1, sc);
            } else {
              i = 0;
              kbd_print_scancode(!(scancode & BIT(7)), 2, sc);
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
  if (kbd_print_no_sysinb(cnt)) return 1;

  return 0;
}
