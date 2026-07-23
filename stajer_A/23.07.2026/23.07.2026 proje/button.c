#include "button.h"
#include "hal_pcu.h"

static uint32_t press_start_time = 0;
static bool is_pressed = false;
static uint32_t last_debounce_time = 0;
static bool last_pin_state = false;

void Button_Init(void) {
    HAL_PCU_SetInOutMode(PCU_ID_C, PCU_PIN_ID_9, PCU_INOUT_INPUT);
}

static bool HW_ReadButton(void) {
    PCU_PORT_e pin_val;
    HAL_PCU_GetInputValue(PCU_ID_C, PCU_PIN_ID_9, &pin_val);
    return (pin_val == 0); // Active-Low (0 ise basılı)
}

ButtonEvent_t Button_Update(uint32_t current_time_ms) {
    ButtonEvent_t event = EVENT_NONE;
    bool current_pin_state = HW_ReadButton();

    // 1. Debounce (Fiziksel sekme) Kontrolü
    if (current_pin_state != last_pin_state) {
        last_debounce_time = current_time_ms;
    }

    if ((current_time_ms - last_debounce_time) > 20) {
        // 20ms boyunca pin durumu sabit kaldıysa gerçek bir harekettir
        if (current_pin_state != is_pressed) {
            is_pressed = current_pin_state;

            if (is_pressed) {
                // Butona yeni basıldı, kronometreyi başlat
                press_start_time = current_time_ms;
            } else {
                // Buton bırakıldı, süreyi hesapla
                uint32_t duration = current_time_ms - press_start_time;

                if (duration >= 1000) {
                    event = EVENT_LONG_PRESS; // 1 saniyeden uzun
                } else if (duration > 20) {
                    event = EVENT_SHORT_PRESS; // Kısa tık
                }
            }
        }
    }

    last_pin_state = current_pin_state;
    return event;
}
