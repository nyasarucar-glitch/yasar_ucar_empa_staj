#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

typedef enum {
    BTN_NONE,
    BTN_SHORT,
    BTN_LONG
} ButtonEvent_t;

void Button_Init(void);
ButtonEvent_t Button_Update(uint32_t current_time_ms);

#endif /* BUTTON_H */
