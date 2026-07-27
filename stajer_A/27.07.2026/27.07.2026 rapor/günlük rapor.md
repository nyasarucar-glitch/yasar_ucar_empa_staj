# Gün 06 — Görevler (Debug Library)

Önce teoriyi oku: [`01_Anlatim.md`](01_Anlatim.md)  
Sonra bu dosyadaki adımları sırayla yap. **Tek ürün:** Debug Library.

**Kaynaklar:** kart şeması / `Tiremo.Cortex.pdf` · `Examples/UARTn/` · MCUBrew · Tera Term

---

## Ne yapacaksın? (özet)

Kartın **debug UART** hattını şemadan bulacaksın (hangi UARTn, TX/RX pinleri, baud).  
Peripheral’da bu UART’ı yapılandıracaksın.  
Sonra `debug.c` / `debug.h` yazıp şöyle kullanacaksın:

```c
Debug_Init();
Debug_Print("Hello Tiremo\r\n");
```

Tera Term’de aynı yazıyı görmelisin.

---

## Ortak hazırlık

- [ ] [`01_Anlatim.md`](01_Anlatim.md) okundu
- [ ] Kart USB-C ile bağlı
- [ ] Tera Term kurulu
- [ ] Şema / Tiremo dökümanı açık

---

## Adım 1 — Şemadan debug UART’ı bul

Ezberleme; şemadan çıkar.

1. `Tiremo.Cortex.pdf` veya şema PDF’te şunları ara: `UART`, `DEBUG`, `USB`, `TX`, `RX`, `VCP`.  
2. USB-C (debug/seri) tarafındaki neti MCU pinine kadar takip et.  
3. Hangi **UART birimi** (UART0, UART1, …) olduğunu yaz.  
4. **TX pin** ve **RX pin** (port + pin) yaz.  
5. Baud rate’i döküman/örnekten doğrula (sık görülen: **115200**).  
6. Frame: genelde **8N1** (8 data, no parity, 1 stop).

Rapora şu tabloyu doldur (zorunlu):

| Alan | Değer |
|------|-------|
| Debug UART | |
| TX pin | |
| RX pin | |
| Baud rate | |
| Data / parity / stop | |
| COM port (PC) | |

- [ ] Tablo dolduruldu

---

## Adım 2 — Peripheral’da UART yapılandır

MCUBrew32 (veya projedeki peripheral config) ile:

1. Doğru **UART instance**’ı seç (Adım 1’de bulduğun).  
2. **TX** ve **RX** pinlerini doğru porta / alternate function’a map’le.  
3. Baud rate ayarla (ör. 115200).  
4. 8 data bit, parity none, 1 stop bit.  
5. UART clock / enable açık olsun.  
6. Code Generate (gerekirse) → eMStudio32’de **Clean → Build**.  
7. MCUBrew’i pencere **X** ile kapatma (bilinen sorun).

- [ ] Build başarılı

---

## Adım 3 — Debug Library dosyalarını oluştur

Önerilen yapı:

```
Libraries/Debug/   (veya projenin uygun klasörü)
├── debug.h
└── debug.c
```

### `debug.h` — en az bunlar

```c
#ifndef DEBUG_H
#define DEBUG_H

void Debug_Init(void);
void Debug_Print(const char *text);

#endif
```

### `Debug_Init`

- UART board/peripheral init’ten sonra çağrılacak şekilde yaz.  
- Gerekli son hazırlığı burada yap (çoğu zaman init zaten üretilmiş olabilir; yine de API’de dursun).

### `Debug_Print`

String’i **null’a (`\0`) kadar** gönder:

1. `text` NULL ise çık.  
2. Her karakter için: TX hazır olana kadar bekle → karakteri UART’a yaz.  
3. SDK’daki UART send fonksiyonunu kullan (`Uart_Send` vb. — projenizdeki isim neyse).

Pseudocode:

```c
void Debug_Print(const char *text)
{
    if (text == 0) return;
    while (*text != '\0') {
        /* TX ready bekle */
        /* *text gönder */
        text++;
    }
}
```

Satır sonu için Windows terminalde `\r\n` kullan.

- [ ] `debug.h` / `debug.c` eklendi
- [ ] Proje bu dosyaları derliyor

---

## Adım 4 — main’den çağır ve test et

```c
#include "debug.h"

/* board / peripheral init'ten sonra: */
Debug_Init();
Debug_Print("=== Debug Library Test ===\r\n");
Debug_Print("1\r\n");
Debug_Print("2\r\n");
Debug_Print("3\r\n");
```

Tera Term:

1. Doğru COM port  
2. Baud = senin ayarın  
3. 8N1  

Beklenen: üç test satırı + başlık görünür.

- [ ] Terminalde metin göründü
- [ ] En az 3 farklı mesaj basıldı

---

## Checklist (hepsi zorunlu)

| # | Madde | Yapıldı |
|---|-------|---------|
| 1 | Şemadan UARTn bulundu | ☐ |
| 2 | TX / RX pinleri yazıldı | ☐ |
| 3 | Baud + 8N1 net | ☐ |
| 4 | Peripheral yapılandırıldı, build OK | ☐ |
| 5 | `Debug_Init` + `Debug_Print` yazıldı | ☐ |
| 6 | Tera Term’de çıktı görüldü | ☐ |
| 7 | Rapora tablo dolduruldu | ☐ |

### Mentöre gösterirken

1. Rapor tablosu  
2. Kodda `Debug_Print("Hello\r\n");`  
3. Terminalde aynı metin  

---

## Teslim

```
teslimler/Stajyer_X/
├── rapor/gunluk_rapor.md
└── proje/                 ← debug.c / debug.h + proje
```

---

## Takılırsan kontrol et

| Belirti | Bak |
|---------|-----|
| Boş terminal | COM / baud / yanlış UART |
| Çöp karakter | Baud uyuşmazlığı |
| Tek harf | TX ready beklemeden yazmak |
| Build OK log yok | `Debug_Init` sırası / include |
