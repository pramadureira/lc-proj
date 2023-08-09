#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int timer_hook_id = TIMER0_IRQ;
int timer_counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  // The frequency has this constraint, so that newfreq "fits" in 16 bits
  if (timer < 0 || timer > 2 || freq < 19) return 1;

  uint8_t config;
  timer_get_conf(timer, &config);

  config &= 0x0f;

  config |= TIMER_LSB_MSB | (timer << 6);
  
  if (sys_outb(TIMER_CTRL, config)) return 1;

  uint8_t lsb, msb;
  uint16_t newfreq = TIMER_FREQ / freq;
  if (util_get_LSB(newfreq, &lsb)) return 1;
  if (util_get_MSB(newfreq, &msb)) return 1;
  if (sys_outb(TIMER_0 + timer, lsb)) return 1;
  if (sys_outb(TIMER_0 + timer, msb)) return 1;

  return 0;
}

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

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  if (timer < 0 || timer > 2) return 1;

  union timer_status_field_val val;
 
  switch (field) {
  case tsf_all:
    val.byte = st;
    break;
  case tsf_initial:
    val.in_mode = (st & TIMER_LSB_MSB) >> 4;
    break;
  case tsf_mode:
    if ((st & TIMER_SQR_WAVE) == TIMER_RATE_GEN) val.count_mode = 2;
    else if ((st & TIMER_SQR_WAVE) == TIMER_SQR_WAVE) val.count_mode = 3;
    else val.count_mode = (st & (BIT(3) | BIT(2) | BIT(1))) >> 1;
    break;
  case tsf_base:
    val.bcd = st & TIMER_BCD;
    break;
  default:
    return 1;
  }

  if (timer_print_config(timer, field, val)) return 1;

  return 0;
}
