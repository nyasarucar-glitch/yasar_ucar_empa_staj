#include <stdint.h>
#include <stdio.h>
#include "hal_uart.h"

/* USER CODE BEGIN 0 */
// 8 bitlik kontrol register'imiz (Baslangic = 0x00)
uint8_t control_register = 0x00;

// Bit pozisyonlarinin anlasilir olmasi icin isim (macro) tanimlamalari
#define BIT_ENABLE      0
#define BIT_ERROR       1
#define BIT_MODE0       2
#define BIT_MODE1       3
#define BIT_DATA_READY  4

// Tera Term (UART) icin zorunlu yazdirma koprusu
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        HAL_UART_Transmit(UART_ID_0, (uint8_t *)&ptr[i], 1, true);
    }
    return len;
}
/* USER CODE END 0 */


/* USER CODE BEGIN 1 */

// Ekranın akıp gitmesini engelleyecek bekleme döngüsü
void delay_ms(volatile uint32_t count) {
    for(uint32_t i = 0; i < (count * 1000); i++) {
        __asm("nop");
    }
}

// Senaryodaki tum islemleri gerceklestiren gorev fonksiyonu
void run_register_scenario(void) {
    // 0. Baslangic Durumu
    control_register = 0x00;
    printf("Initial Register = 0x%02X\r\n\n", control_register);
    delay_ms(5000);

    // 1. ENABLE bitini aktif et (SET)
    control_register |= (1 << BIT_ENABLE);
    printf("ENABLE SET\r\n");
    printf("Register = 0x%02X\r\n\n", control_register);
    delay_ms(5000);

    // 2. ERROR bitini temizle (CLEAR) ve MODE degistir (Maskeleme ile)
    // Gorev Maddesi 6: "Maskeleme kullanarak birden fazla biti degistir"
    // Tek satirda hem ERROR bitini(1. bit) hem de MODE bitlerini(2. ve 3. bit) '0' yapiyoruz.
    control_register &= ~((1 << BIT_ERROR) | (1 << BIT_MODE0) | (1 << BIT_MODE1));

    // Ardindan sadece MODE1'i '1' yaparak "TX Modu"na (0x09) geciyoruz
    control_register |= (1 << BIT_MODE1);

    printf("MODE = TX\r\n");
    printf("Register = 0x%02X\r\n\n", control_register);
    delay_ms(5000);

    // 3. DATA_READY bitini toggle et (TOGGLE)
    control_register ^= (1 << BIT_DATA_READY);
    printf("DATA_READY TOGGLE\r\n");
    printf("Register = 0x%02X\r\n\n", control_register);
    delay_ms(5000);

    // 4. Durum Kontrolu (READ)
    // Gorev Maddesi 5: "Bit READ islemi ile durum kontrolu yap"
    if(control_register & (1 << BIT_ENABLE)) {
        printf("[Durum Okuma] -> Cihaz su an AKTIF (ENABLE = 1)\r\n");
    } else {
        printf("[Durum Okuma] -> Cihaz su an KAPALI (ENABLE = 0)\r\n");
    }

    printf("--------------------------------\r\n\n");
    delay_ms(3000);
}

/* USER CODE END 1 */


/*
   DİKKAT: main.c'nin aradigi ana fonksiyon.
*/
void PRV_USER_Code(void)
{
    /* USER CODE BEGIN PRV_USER_Code */

    delay_ms(3000); // Tera Term'e baglanma payi

    while(1) {
        // Senaryoyu sonsuz dongude calistir
        run_register_scenario();
    }

    /* USER CODE END PRV_USER_Code */
}
