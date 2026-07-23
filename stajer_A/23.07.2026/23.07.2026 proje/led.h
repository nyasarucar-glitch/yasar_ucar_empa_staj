#ifndef LED_H
#define LED_H

#include <stdbool.h>

void Led_Init(void);
void Led_SelfTest(void);
bool Led_ChaseStep(bool reverse);

#endif /* LED_H */
