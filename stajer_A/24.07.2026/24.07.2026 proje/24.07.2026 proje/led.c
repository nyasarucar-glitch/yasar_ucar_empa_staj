#include "led.h"
#include "hal_pcu.h"
#include <stdint.h>

#define LED_COUNT 3

typedef struct {
    PCU_ID_e port;
    PCU_PIN_ID_e pin;
} LedPin_t;

static const LedPin_t led_pins[LED_COUNT] = {
    {PCU_ID_B, PCU_PIN_ID_4},
    {PCU_ID_B, PCU_PIN_ID_9},
    {PCU_ID_F, PCU_PIN_ID_7}
};

static uint8_t active_index = 0;

static void DelayMs(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile int j = 0; j < 15000; j++) {}
    }
}

void Led_Init(void) {
    for (int i = 0; i < LED_COUNT; i++) {
        HAL_PCU_SetInOutMode(led_pins[i].port, led_pins[i].pin, PCU_INOUT_OUTPUT_PUSH_PULL);
        HAL_PCU_SetOutputValue(led_pins[i].port, led_pins[i].pin, 1); // Başlangıçta söndür (Active-Low)
    }
}

void Led_SelfTest(void) {
    for (int i = 0; i < LED_COUNT; i++) {
        HAL_PCU_SetOutputValue(led_pins[i].port, led_pins[i].pin, 0); // Yak
        DelayMs(150);
        HAL_PCU_SetOutputValue(led_pins[i].port, led_pins[i].pin, 1); // Söndür
    }
}

bool Led_ChaseStep(bool reverse) {
    // Önce hepsini söndür
    for (int i = 0; i < LED_COUNT; i++) {
        HAL_PCU_SetOutputValue(led_pins[i].port, led_pins[i].pin, 1);
    }

    // Sadece aktif olanı yak
    HAL_PCU_SetOutputValue(led_pins[active_index].port, led_pins[active_index].pin, 0);

    // Çarpışma ve Yön Kontrolü
    if (reverse) {
        if (active_index == 0) {
            reverse = false;
            active_index++;
        } else {
            active_index--;
        }
    } else {
        if (active_index >= LED_COUNT - 1) {
            reverse = true;
            active_index--;
        } else {
            active_index++;
        }
    }
    return reverse;
}

void Led_AllOff(void) {
    for (int i = 0; i < LED_COUNT; i++) {
        HAL_PCU_SetOutputValue(led_pins[i].port, led_pins[i].pin, 1);
    }
}
