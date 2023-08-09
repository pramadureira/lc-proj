#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "../lab2/i8254.h"

static int timer_hook_id = TIMER0_IRQ;
int timer_counter = 0;


int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = timer_hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != OK) return 1;
  return 0;
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)() {
  timer_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (timer < 0 || timer > 2 || st == NULL) return 1;

  uint8_t rb_command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, rb_command)) return 1;

  if (util_sys_inb(TIMER_0 + timer, st)) return 1;

  return 0;
}
