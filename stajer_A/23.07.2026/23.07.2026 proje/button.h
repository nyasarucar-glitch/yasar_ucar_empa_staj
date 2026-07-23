#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

// Buton Olayları (Event Driven Architecture)
typedef enum {
    EVENT_NONE,
    EVENT_SHORT_PRESS,
    EVENT_LONG_PRESS
} ButtonEvent_t;

void Button_Init(void);
ButtonEvent_t Button_Update(uint32_t current_time_ms);

#endif /* BUTTON_H */
