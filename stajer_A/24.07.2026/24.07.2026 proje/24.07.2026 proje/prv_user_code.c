#include <stdint.h>
#include <stdbool.h>
#include "led.h"
#include "button.h"

typedef enum {
    MODE_CHASE_SLOW,
    MODE_CHASE_FAST,
    MODE_PAUSE
} AppMode_t;

static uint32_t Get_System_Time_ms(void) {
    static uint32_t tick = 0;
    for (volatile int i = 0; i < 15000; i++) {} // ~1ms
    return tick++;
}

void PRV_USER_Code(void) {
    // İskelet Kurulumu
    Led_Init();
    Button_Init();
    Led_SelfTest();

    // Başlangıç Durumları
    AppMode_t app_mode = MODE_CHASE_SLOW;

    bool reverse = false;
    uint32_t last_chase_time = 0;
    uint32_t chase_delay_ms = 250;

    while (1) {
        uint32_t current_time = Get_System_Time_ms();

        ButtonEvent_t ev = Button_Update(current_time);

        // Mod Değişimleri
        if (ev == BTN_SHORT) {
            if (app_mode == MODE_CHASE_SLOW) {
                app_mode = MODE_CHASE_FAST;
            } else if (app_mode == MODE_CHASE_FAST) {
                app_mode = MODE_CHASE_SLOW;
            } else if (app_mode == MODE_PAUSE) {
                app_mode = MODE_CHASE_SLOW;
            }
        }
        else if (ev == BTN_LONG) {
            if (app_mode == MODE_PAUSE) {
                app_mode = MODE_CHASE_SLOW;
            } else {
                app_mode = MODE_PAUSE;
            }
        }

        // Durum Makinesi (State Machine)
        switch (app_mode) {

            case MODE_CHASE_SLOW:
                chase_delay_ms = 250;
                if ((current_time - last_chase_time) >= chase_delay_ms) {
                    last_chase_time = current_time;
                    reverse = Led_ChaseStep(reverse);
                }
                break;

            case MODE_CHASE_FAST:
                chase_delay_ms = 80;
                if ((current_time - last_chase_time) >= chase_delay_ms) {
                    last_chase_time = current_time;
                    reverse = Led_ChaseStep(reverse);
                }
                break;

            case MODE_PAUSE:
                Led_AllOff();
                last_chase_time = current_time;
                break;

            default:
                app_mode = MODE_CHASE_SLOW;
                break;
        }
    }
}
