#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

// Yeni olaylarımız: Tıklama, 1 Saniye ve 3 Saniye
typedef enum {
    EVENT_NONE,
    EVENT_SHORT_CLICK, // Kapatmak için kullanılacak
    EVENT_PRESS_1S,    // 1 Saniye doldu
    EVENT_PRESS_3S     // 3 Saniye doldu
} ButtonEvent_t;

// State Machine'e 1 ve 3 saniye bekleme durumlarını ekledik
typedef enum {
    STATE_IDLE,
    STATE_DEBOUNCE_PRESS,
    STATE_WAIT_1S,
    STATE_WAIT_3S,
    STATE_WAIT_RELEASE,
    STATE_DEBOUNCE_RELEASE
} ButtonState_t;

// Fonksiyon Prototipleri
void Button_Init(void);
ButtonEvent_t Button_Process(uint32_t current_time_ms);
extern bool HW_ReadButton(void);

#endif /* BUTTON_H */
