#include <stdint.h>
#include "hal_uart.h"
#include "abov_config.h"

void PRV_USER_Code(void) {
    UART_CFG_t stCfg = {
        .un32BaudRate = 115200,
        .eData      = UART_DATA_8,
        .eParity    = UART_PARITY_NONE,
        .eStop      = UART_STOP_1
    };

    HAL_UART_Init((UART_ID_e)DEBUG_UART_ID);
    HAL_UART_SetConfig((UART_ID_e)DEBUG_UART_ID, &stCfg);

    while (1) {
        char *text = "merhaba\r\n";
        while (*text != '\0') {
            uint8_t ch = (uint8_t)(*text);
            HAL_UART_Transmit((UART_ID_e)DEBUG_UART_ID, &ch, 1, true);
            text++;
        }

        for (volatile uint32_t i = 0; i < 150000000; i++) {
        }
    }
}
