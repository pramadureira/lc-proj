// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include <lcom/timer.h>
#include "mouse.h"
#include "macro.h"
#include "state.h"

extern uint8_t read_byte;
extern int ret_val;
extern int timer_counter;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    uint8_t bit_no;
    int ipc_status, r = 0, i = 0;
    message msg;
    uint8_t packets[3];

    if (mouse_data_reporting(MS_ENABLE_REP)) return 1;
    if (mouse_subscribe_int(&bit_no)) return 1;

    while (cnt > 0) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d", r);
        continue;
        }

        if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:                           
            if (msg.m_notify.interrupts & BIT(bit_no)) {
                mouse_ih();
                if (ret_val == 1) {
                    ret_val = 0;
                    i = 0;
                    continue;
                }

                packets[i] = read_byte;
                switch (i) {
                    case 0:
                        if (!(read_byte & MS_FIRST_BYTE)) {
                            continue;
                        }
                        i++;
                        break;
                    case 1:
                        i++;
                        break;
                    case 2:
                        i = 0;
                        struct packet pp = mouse_parse_packet(packets);
                        mouse_print_packet(&pp);
                        cnt--;
                        break;
                    default:
                        break;
                }
            }
            break;
            default:
            break;
        }
        }
    }

    if (mouse_unsubscribe_int()) return 1;
    if (mouse_data_reporting(MS_DISABLE_REP)) return 1;

    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    uint8_t bit_no1, bit_no2, remTime = idle_time;
    int ipc_status, r = 0, i = 0;
    message msg;
    uint8_t packets[3];

    if (mouse_data_reporting(MS_ENABLE_REP)) return 1;
    if (mouse_subscribe_int(&bit_no1)) return 1;
    if (timer_subscribe_int(&bit_no2)) return 1;

    while (remTime > 0) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d", r);
        continue;
        }

        if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:                           
            if (msg.m_notify.interrupts & BIT(bit_no1)) {
                mouse_ih();
                if (ret_val == 1) {
                    ret_val = 0;
                    i = 0;
                    continue;
                }

                packets[i] = read_byte;
                switch (i) {
                    case 0:
                        if (!(read_byte & MS_FIRST_BYTE)) {
                            continue;
                        }
                        i++;
                        break;
                    case 1:
                        i++;
                        break;
                    case 2:
                        i = 0;
                        struct packet pp = mouse_parse_packet(packets);
                        mouse_print_packet(&pp);
                        remTime = idle_time;
                        timer_counter = 0;
                        continue;
                        break;
                    default:
                        break;
                }
            }            

            if (msg.m_notify.interrupts & BIT(bit_no2)) {
                timer_int_handler();
                if (timer_counter % sys_hz() == 0) remTime--;
            }
            break;
            default:
            break;
        }
        }
    }

    if (mouse_unsubscribe_int()) return 1;
    if (timer_unsubscribe_int()) return 1;
    if (mouse_data_reporting(MS_DISABLE_REP)) return 1;

    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    uint8_t bit_no;
    int ipc_status, r = 0, i = 0;
    message msg;
    uint8_t packets[3];
    gesture_state state = INIT;

    if (mouse_data_reporting(MS_ENABLE_REP)) return 1;
    if (mouse_subscribe_int(&bit_no)) return 1;

    while (state != END) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d", r);
        continue;
        }

        if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:                           
            if (msg.m_notify.interrupts & BIT(bit_no)) {
                mouse_ih();
                if (ret_val == 1) {
                    ret_val = 0;
                    i = 0;
                    continue;
                }

                packets[i] = read_byte;
                switch (i) {
                    case 0:
                        if (!(read_byte & MS_FIRST_BYTE)) {
                            continue;
                        }
                        i++;
                        break;
                    case 1:
                        i++;
                        break;
                    case 2:
                        i = 0;
                        struct packet pp = mouse_parse_packet(packets);
                        gesture_handler(&pp, x_len, tolerance, state);
                        change_state(&state);
                        mouse_print_packet(&pp);
                        break;
                    default:
                        break;
                }
            }
            break;
            default:
            break;
        }
        }
    }
    
    if (mouse_unsubscribe_int()) return 1;
    if (mouse_data_reporting(MS_DISABLE_REP)) return 1;

    return 0;
}
