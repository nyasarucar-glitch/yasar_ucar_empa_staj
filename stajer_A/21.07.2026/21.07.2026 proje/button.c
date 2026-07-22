#include "button.h"

static ButtonState_t currentState = STATE_IDLE;
static uint32_t stateTimer = 0;

void Button_Init(void) {
    currentState = STATE_IDLE;
}

ButtonEvent_t Button_Process(uint32_t current_time_ms) {
    ButtonEvent_t event = EVENT_NONE;
    bool isPressed = HW_ReadButton();

    switch (currentState) {
        case STATE_IDLE:
            if (isPressed) {
                currentState = STATE_DEBOUNCE_PRESS;
                stateTimer = current_time_ms;
            }
            break;

        case STATE_DEBOUNCE_PRESS:
            if ((current_time_ms - stateTimer) >= 50) { // 50ms sekme süresi
                if (isPressed) {
                    currentState = STATE_WAIT_1S;
                    stateTimer = current_time_ms;
                } else {
                    currentState = STATE_IDLE;
                }
            }
            break;

        case STATE_WAIT_1S:
            if (!isPressed) {
                // 1 saniye dolmadan parmak çekildiyse bu bir kısa tıklamadır (Kapatma)
                event = EVENT_SHORT_CLICK;
                currentState = STATE_DEBOUNCE_RELEASE;
                stateTimer = current_time_ms;
            } else if ((current_time_ms - stateTimer) >= 1000) {
                // 1 Saniye Doldu!
                event = EVENT_PRESS_1S;
                currentState = STATE_WAIT_3S; // Artık 3. saniyeyi beklemeye geç
            }
            break;

        case STATE_WAIT_3S:
            if (!isPressed) {
                currentState = STATE_DEBOUNCE_RELEASE;
                stateTimer = current_time_ms;
            } else if ((current_time_ms - stateTimer) >= 3000) {
                // Toplam 3 Saniye Doldu!
                event = EVENT_PRESS_3S;
                currentState = STATE_WAIT_RELEASE; // Buton bırakılana kadar bekle
            }
            break;

        case STATE_WAIT_RELEASE:
            if (!isPressed) {
                currentState = STATE_DEBOUNCE_RELEASE;
                stateTimer = current_time_ms;
            }
            break;

        case STATE_DEBOUNCE_RELEASE:
            if ((current_time_ms - stateTimer) >= 50) {
                if (!isPressed) {
                    currentState = STATE_IDLE;
                }
            }
            break;
    }

    return event;
}
