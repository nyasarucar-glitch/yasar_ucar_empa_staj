#include "button.h"
#include "hal_pcu.h"
#include <stdbool.h>

static uint32_t press_start_time = 0;
static bool is_pressed = false;
static uint32_t last_debounce_time = 0;
static bool last_pin_state = false;

void Button_Init(void) {
    HAL_PCU_SetInOutMode(PCU_ID_C, PCU_PIN_ID_9, PCU_INOUT_INPUT);
}

ButtonEvent_t Button_Update(uint32_t current_time_ms) {
    ButtonEvent_t event = BTN_NONE;

    PCU_PORT_e pin_val;
    HAL_PCU_GetInputValue(PCU_ID_C, PCU_PIN_ID_9, &pin_val);
    bool current_pin_state = (pin_val == 0); // 0 ise basılı (Active-Low)

    // Debounce Filtresi
    if (current_pin_state != last_pin_state) {
        last_debounce_time = current_time_ms;
    }

    if ((current_time_ms - last_debounce_time) > 20) {
        if (current_pin_state != is_pressed) {
            is_pressed = current_pin_state;

            if (is_pressed) {
                press_start_time = current_time_ms;
            } else {
                uint32_t duration = current_time_ms - press_start_time;

                if (duration >= 1000) {
                    event = BTN_LONG;
                } else if (duration > 20) {
                    event = BTN_SHORT;
                }
            }
        }
    }
    last_pin_state = current_pin_state;
    return event;
}
