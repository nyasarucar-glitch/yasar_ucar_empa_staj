#include <stdint.h>
#include <stdbool.h>
#include "led.h"
#include "button.h"

// Non-blocking sistem saati (Tick)
uint32_t Get_System_Time_ms(void) {
    static uint32_t dummy_tick = 0;
    for (volatile int i = 0; i < 15000; i++) {} // ~1ms
    return dummy_tick++;
}

void PRV_USER_Code(void) {
    // --- SİSTEM BAŞLATMA (INIT) ---
    Led_Init();
    Button_Init();

    // 1. AÇILIŞ DAVRANIŞI: LED'ler sırayla bir kez yanıp söner
    Led_SelfTest();

    // --- DEĞİŞKENLER ---
    bool reverse = false;
    bool fast_mode = false;
    uint32_t last_chase_time = 0;

    const uint32_t CHASE_SLOW_MS = 250;
    const uint32_t CHASE_FAST_MS = 80;

    // 2. ANA DÖNGÜ (Sürekli Çalışan Mimari)
    while (1) {
        uint32_t current_time = Get_System_Time_ms();

        // 3. BUTON DAVRANIŞI (Debounce ve Süre ölçümü burada işlenir)
        ButtonEvent_t event = Button_Update(current_time);

        // 4. OKUNABİLİR KARAR AĞACI (Switch / Else If)
        if (event == EVENT_SHORT_PRESS) {
            reverse = !reverse; // Kısa basış: Yönü tersine çevir
        }
        else if (event == EVENT_LONG_PRESS) {
            fast_mode = !fast_mode; // Uzun basış: Hız modunu değiştir
        }

        // Seçilen moda göre hız limitini belirle
        uint32_t current_speed = fast_mode ? CHASE_FAST_MS : CHASE_SLOW_MS;

        // 5. CHASE (Kayan Işık) GÜNCELLEMESİ
        if ((current_time - last_chase_time) >= current_speed) {
            last_chase_time = current_time;

            // Adımı işlet ve gerekirse duvara çarpma durumunu(reverse) güncelle
            reverse = Led_ChaseStep(reverse);
        }
    }
}
