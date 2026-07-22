#include "button.h"
#include <stdint.h>
#include <stdbool.h>
#include "hal_pcu.h"

// --- DONANIM SOYUTLAMA KATMANI (HAL) ---

bool HW_ReadButton(void) {
    PCU_PORT_e pin_val;
    HAL_PCU_GetInputValue(PCU_ID_C, PCU_PIN_ID_9, &pin_val);
    return (pin_val == 0);
}

// Bütün LED'leri kontrol eden fonksiyon
void HW_SetAllLEDs(bool state) {
    PCU_PORT_e val = state ? (PCU_PORT_e)0 : (PCU_PORT_e)1;

    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_4,  val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_9,  val);
    HAL_PCU_SetOutputValue(PCU_ID_F, PCU_PIN_ID_7,  val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_5,  val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_10, val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_11, val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_12, val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_13, val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_14, val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_15, val);
}

// Sadece TEK BİR LED'i (LED1) kontrol eden fonksiyon
void HW_SetOneLED(bool state) {
    PCU_PORT_e val = state ? (PCU_PORT_e)0 : (PCU_PORT_e)1;
    PCU_PORT_e off_val = (PCU_PORT_e)1; // Diğerlerini söndürmek için

    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_4, val);  // Sadece LED1 hedefleniyor

    // Kalan tüm LED'leri kapat
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_9,  off_val);
    HAL_PCU_SetOutputValue(PCU_ID_F, PCU_PIN_ID_7,  off_val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_5,  off_val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_10, off_val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_11, off_val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_12, off_val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_13, off_val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_14, off_val);
    HAL_PCU_SetOutputValue(PCU_ID_B, PCU_PIN_ID_15, off_val);
}

// Sistem zamanlayıcısı (Yapay Gecikme ile Düzeltildi)
uint32_t Get_System_Time_ms(void) {
    static uint32_t dummy_tick = 0;

    // İŞLEMCİYİ YAVAŞLATMA DÖNGÜSÜ (Yaklaşık 1 milisaniye)
    // Eğer 1 saniye çok hızlı dolarsa bu '15000' sayısını artır, çok yavaşsa azalt.
    for (volatile int i = 0; i < 15000; i++) {
        // Döngü içi boş, sadece işlemciyi meşgul ediyoruz.
    }

    return dummy_tick++;
}

// IDE'nin main.c'den çağırdığı asıl kullanıcı fonksiyonu
void PRV_USER_Code(void) {

    // --- PİN BAŞLANGIÇ AYARLARI (INIT) ---
    HAL_PCU_SetInOutMode(PCU_ID_C, PCU_PIN_ID_9, PCU_INOUT_INPUT); // Buton

    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_4,  PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_9,  PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_F, PCU_PIN_ID_7,  PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_5,  PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_10, PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_11, PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_12, PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_13, PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_14, PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetInOutMode(PCU_ID_B, PCU_PIN_ID_15, PCU_INOUT_OUTPUT_PUSH_PULL);

    Button_Init();
    HW_SetAllLEDs(false); // Başlangıçta her şey kapalı

    while (1) {
        uint32_t current_time = Get_System_Time_ms();
        ButtonEvent_t current_event = Button_Process(current_time);

        switch (current_event) {
            case EVENT_SHORT_CLICK:
                // Tıklayıp çekersen sistemi sıfırlar, her şeyi söndürür.
                HW_SetAllLEDs(false);
                break;

            case EVENT_PRESS_1S:
                // 1 Saniye dolduğunda tek LED yanar.
                HW_SetOneLED(true);
                break;

            case EVENT_PRESS_3S:
                // 3 Saniye dolduğunda (parmak hala basılıyken) hepsi yanar.
                HW_SetAllLEDs(true);
                break;

            case EVENT_NONE:
            default:
                break;
        }
    }
}
