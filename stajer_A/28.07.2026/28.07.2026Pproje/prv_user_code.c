#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "hal_uart.h"

extern void PRV_SCU_CLK_Init(void);
extern void PRV_UART_Init(void);

void PRV_USER_Code(void)
{
    uint32_t raw_value = 0;
    uint32_t voltage_mv = 0;
    char buffer[128];

    // Temel donanımları açıyoruz
    PRV_SCU_CLK_Init();
    PRV_UART_Init();

    // ---------------------------------------------------------
    // SENİN REGISTER KODLARIN (Code Generator'ı Devreden Çıkarır)
    // ---------------------------------------------------------
    // SCU üzerinden ADC0 Clock Aktif
    *((volatile uint32_t*)0x41010034) |= (1 << 20);
    *((volatile uint32_t*)0x4101002C) |= (1 << 20);

    // PA2 - AN2 Pin Ayarı
    *((volatile uint32_t*)0x41010000) |= (7 << 8);
    *((volatile uint32_t*)0x4101000C) &= ~(3 << 4);

    // ADC0 Yapılandırma
    *((volatile uint32_t*)0x41012000) = 0x00000002;
    *((volatile uint32_t*)0x41012020) = 0x00000001;
    // ---------------------------------------------------------

    for (volatile uint32_t i = 0; i < 2000000; i++);

    char init_msg[] = "\r\n--- Pil Olcum Sistemi Basladi (Register Mode) ---\r\n";
    HAL_UART_Transmit(UART_ID_0, (uint8_t*)init_msg, strlen(init_msg), 1000);

    while (1)
    {
        // 1. Çevrimi Başlat
        *((volatile uint32_t*)0x41012020) |= (1 << 1);

        // 2. Bit 0 Busy kontrolü ile çevrimin bitmesini bekle
        volatile uint32_t timeout = 100000;
        while (((*((volatile uint32_t*)0x41012024)) & (1 << 0)) && --timeout);

        // 3. Veriyi Çek (Sadece ilk 12 biti alıyoruz - 0x0FFF)
        raw_value = (*((volatile uint32_t*)0x41012030)) & 0x0FFF;

        // 4. Voltajı Hesapla
        voltage_mv = (raw_value * 3300) / 4095;

        // 5. UART ile Terminale Bas
        sprintf(buffer, "Register OK | BAT = %lu mV (raw: %lu)\r\n", voltage_mv, raw_value);
        HAL_UART_Transmit(UART_ID_0, (uint8_t*)buffer, strlen(buffer), 1000);

        // Bir sonraki ölçüm için bekleme
        for (volatile uint32_t i = 0; i < 15000000; i++);
    }
}
