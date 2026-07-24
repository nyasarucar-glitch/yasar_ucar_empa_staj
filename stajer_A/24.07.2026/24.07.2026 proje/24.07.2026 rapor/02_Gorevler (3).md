# Gün 05 — Görevler

Hafta 1 mini proje (uygulama günü)

Cuma **entegrasyon günü**: yeni uzun teori yok. Pazartesi’den Perşembe’ye kadar öğrendiklerini **tek çalışan demo** haline getiriyorsun. Kod yazma süresi günün büyük kısmı.

Kısa özet not: [`01_Anlatim.md`](01_Anlatim.md)  
Sıra: **Kolay → Orta → Zor**.

---

## Hafta 1’de elinde ne olmalı?

| Gün | Kart / kavram | Cuma’da projede |
|-----|---------------|-----------------|
| Pazartesi | GPIO, LED, buton | Pin init, yak/söndür |
| Salı | MCU, RAM/Flash, debounce | `const`, `uint32_t`, state |
| Çarşamba | Kesme, NVIC, flag | `volatile`, `if (flag)` |
| Perşembe | `for`, `while`, `if`, dosya | Chase, self-test, modüler kod |

Eksik bir gün varsa önce Perşembe kolay/orta görevlerini tamamlayıp Cuma’ya geç.

---

## Ortak hazırlık

- [x] Kart ve IDE hazır
- [x] En az **3 LED** ve **1 buton** pin’i belli
- [x] Perşembe chase veya blink kodun var (kopyalayıp geliştirebilirsin)
- [x] GitHub staj repon açık (Cuma zor için)

---

## Kolay

### Amaç

Haftanın parçalarını **tek `main.c` içinde** üst üste koymak. Amaç “güzel mimari” değil; **hepsinin bir arada çalıştığını** görmek.

| | |
|---|---|
| **Görev** | Self-test + ana mod + buton tepkisi |
| **Yapıldı** | x |

### Program akışı (kartta göreceğin sıra)

```
Güç / reset
    ↓
for → LED’ler sırayla bir kez yan-sön  (self-test)
    ↓
while(1) sonsuz döngü
    ├── buton oku (poll veya flag)
    ├── if → bir şey değiştir (toggle / mod)
    └── chase veya blink (for veya if ile LED)
```

### Adım adım

**1. Self-test (`for`)**

- `LED_COUNT` kadar LED.  
- Döngüde her birini 100–200 ms yak, söndür.  
- Bittiğinde normal moda geç.

**2. Ana mod (`while(1)`)**

- **Chase:** Perşembe orta görevindeki `Chase_Step` benzeri.  
- **veya Blink:** Perşembe kolay görevindeki `led_on` toggle.

**3. Buton (`if`)**

- Polling: `if (Gpio_Read(BTN) == basili)` → LED toggle veya `reverse = !reverse`.  
- **veya** kesme: `if (button_flag)` → flag temizle → aynı iş.

**4. Minimum C kontrol listesi**

- [x] En az **1** `for` (self-test veya chase içinde)  
- [x] En az **2** `if` veya `else`  
- [x] **1** `while(1)`  
- [x] **1** sayaç: `uint8_t` / `uint32_t` (index, tick, vb.)

### Doğru çalışıyor mu?

- [x] Reset sonrası LED’ler bir kez sırayla yanıp sönüyor.  
- [x] Sonra sürekli chase veya blink var.  
- [x] Butona basınca davranış **gözle görülür** şekilde değişiyor.  
- [x] Program kilitlenmiyor (ISR’da uzun delay yok).

---

## Orta

### Amaç

Kodu **dosyalara bölmek** ve **üç kullanıcı senaryosu** tanımlamak. `main` sadece “orkestra şefi” olur; LED ve buton ayrı modüllerde.

| | |
|---|---|
| **Görev** | `led.c` + `button.c` + 3 senaryo |
| **Yapıldı** | x |

### Önerilen dosya yapısı

```
proje/
├── main.c
├── led.h
├── led.c
├── button.h
└── button.c
```

| Dosya | Fonksiyon örnekleri | Ne iş yapar? |
|-------|---------------------|--------------|
| `led.c` | `Led_Init`, `Led_SelfTest`, `Led_ChaseStep` | Pin’ler, for döngüsü, chase index |
| `button.c` | `Button_Init`, `Button_Update` veya flag okuma | Debounce veya EXTI init |
| `main.c` | `main` | Init + `while(1)` + senaryolara tepki |

### Üç senaryo — ne demek?

Kartta **gösterebileceğin** üç farklı kullanım. Örnek set:

| # | Senaryo | Nasıl tetiklenir? | Beklenen sonuç |
|---|---------|-------------------|----------------|
| 1 | Normal chase | (varsayılan) | LED’ler sırayla akar |
| 2 | Kısa basış / tek basış | Buton | Bir LED toggle **veya** yön değişir |
| 3 | Uzun basış **veya** ikinci davranış | Basılı tut ≥ ~1 sn | Chase hızlanır / yavaşlar / tüm LED flash |

UART yoksa 3. senaryo “uzun basış” olabilir; UART varsa isteğe bağlı `printf("mode=2\n")` ekleyebilirsin.

### `main` iskeleti (örnek)

```c
int main(void)
{
    System_Init();
    Led_Init();
    Button_Init();

    Led_SelfTest();

    while (1) {
        ButtonEvent_t ev = Button_Update();   /* veya flag kontrolü */

        if (ev == BTN_SHORT) {
            /* senaryo 2 */
        } else if (ev == BTN_LONG) {
            /* senaryo 3 */
        }

        Led_ChaseStep();
        DelayMs(chase_delay_ms);
    }
}
```

### Adım adım

1. Önce kolay görevi çalışır halde bırak.  
2. `Led_ChaseStep` ve `Led_SelfTest`’i `led.c`’ye taşı; `led.h`’de prototip.  
3. Buton kodunu `button.c`’ye taşı.  
4. `main.c`’de sadece çağrılar kalsın — mümkünse **40 satır civarı**.  
5. Üç senaryoyu kartta sırayla dene; rapora hangi basışın ne yaptığını yaz.

### Doğru çalışıyor mu?

- [x] Proje derleniyor, `#include "led.h"` / `button.h` doğru.  
- [x] Self-test + chase çalışıyor.  
- [x] Salı **debounce** veya Çarşamba **kesme** projede (en az biri).  
- [x] Üç senaryo kartta gösterilebilir.  
- [x] `for` chase veya self-test’te kullanılıyor.

---

## Zor

### Amaç

**Mini panel** hissi: programın bir **modu** var (`enum`), buton modu değiştiriyor (`switch`). Hafta 1 demosu + GitHub + kısa sunum provası.

| | |
|---|---|
| **Görev** | `enum` mod + demo + `2026-07-24/` |
| **Yapıldı** | x |

### Mod kavramı

Tek bir `bool` yerine “uygulama şu an ne yapıyor?” sorusuna cevap:

```c
typedef enum {
    MODE_CHASE_SLOW,
    MODE_CHASE_FAST,
    MODE_PAUSE
} AppMode_t;

static AppMode_t app_mode = MODE_CHASE_SLOW;
```

`while(1)` içinde:

```c
switch (app_mode) {
case MODE_CHASE_SLOW:
    chase_delay_ms = 200;
    Led_ChaseStep();
    break;
case MODE_CHASE_FAST:
    chase_delay_ms = 80;
    Led_ChaseStep();
    break;
case MODE_PAUSE:
    Led_AllOff();
    break;
default:
    break;
}
```

Buton olayları modu değiştirir:

```c
if (ev == BTN_SHORT) {
    /* örn. slow ↔ fast */
} else if (ev == BTN_LONG) {
  app_mode = MODE_PAUSE;  /* veya pause'tan çık */
}
```

### Zorunlu teknik liste

- [ ] `enum` + `switch` **veya** uzun `if / else if` zinciri  
- [ ] `for` (self-test veya chase)  
- [ ] `while(1)`  
- [ ] `const` en az bir süre/eşik sabiti  
- [ ] Debounce **veya** kesme + (`volatile` gerekirse)  
- [ ] Modüler dosya (orta görev gibi)  
- [ ] GitHub `2026-07-24/` + README  

### Demo provası (8–10 dk)

Kendine veya arkadaşına anlatır gibi sırayla:

1. **Reset** → “Şimdi self-test; `for` ile her LED’i kontrol ediyorum.”  
2. **Normal** → “Chase yavaş modda; `while` içinde `Led_ChaseStep`.”  
3. **Kısa basış** → “Mod hızlandı / yön değişti.”  
4. **Uzun basış** → “Pause veya fast mod.”  
5. (Varsa) UART satırı.

Demo sırasında **hangi C yapısının nerede olduğunu** bir cümleyle söylemen rapora da yazılacak.

### GitHub README’de olması iyi olanlar

- Proje adı ve 2 cümle özet  
- Hafta 1 tablosu: hangi özellik hangi günden  
- C yapıları listesi (`for` → …, `volatile` → …)  
- Derleme / kart notu (kısa)

### Doğru çalışıyor mu?

- [x] En az 2 mod arasında geçiş butonla yapılabiliyor.  
- [x] Pause modunda chase duruyor veya LED’ler sönük.  
- [x] Kod okunaklı; `main` şişmemiş.  
- [x] Repo linki raporda.

---

## Rapora yaz (Cuma)

C programla videolarında iç içe döngü videoları izledim. int sayı=0 ifadesinin kullanıcadan beklenmesine rağmen neden 0a eşitlediğimiz kafamı karıştırdı. kod runtime da doğru çalışsa bile complite timeda bellekte rastgale dolanmasın , belli bir yeri olsun diye eşilediğimizi öğrendim. runtime ; kodun bilgisayar dilinde kodlanıp yüklendiği an, complite time kodun yüklendip çalıştığı andır.

### 1. Hafta tablosu (örnek format)

| Gün | Projede karşılığı |
|-----|-------------------|
| Pazartesi | `Led_Init`, GPIO output |
| Salı | `DEBOUNCE_MS`, `down_ms` |
| Çarşamba | `button_flag`, ISR |
| Perşembe | `for` self-test, `Chase_Step` |
| Cuma | `app_mode`, `switch` |

### 2. C özeti

Her madde için **fonksiyon adı** yeter:

- `for` → Led_Init, Led_SelfTest, Led_ChaseStep, Led_AllOff
- `while` → PRV_USER_Code (Sonsuz ana döngü)
- `if` → Button_Update (Süre/Debounce kontrolü), Led_ChaseStep (Sınır/Yön kontrolü) 
- `switch` → PRV_USER_Code (Buton olaylarına göre modlar arası geçiş ve State Machine)
- `const` → led_pins (Donanım haritası dizisinin korunması)
- `volatile` → Get_System_Time_ms ve DelayMs (Derleyici düzenlemesini önleyen zamanlayıcı döngüleri)

### 3. Takıldığın yer

c dilinin anahtar kelimelerini araştırdım. (bool, uint32_t, static)
Kendi yazdığımız yardımcı fonksiyonlarda anlamakta zorlandım, araştırdım.(Get_System_Time_ms , HW_ReadButton , LED_COUNT)
ABOV Donanım (HAL) Kütüphanesine Ait Terimleri araştırdım.

### 4. Link

GitHub: (https://github.com/nyasarucar-glitch/yasar_ucar_empa_staj/tree/main/stajer_A/24.07.2026/24.07.2026%20proje)

---

## Teslim

```
teslimler/Stajyer_X/
├── rapor/gunluk_rapor.md    ← tablo + C özeti + link
└── proje/
```

GitHub (zor): `2026-07-24/`
