# Gün 03 — Görevler

Kesme (interrupt), NVIC ve harici kesme

Herkes aynı sırayı izler: **Kolay → Orta → Zor**. Bitince çalışmanı göster ve teslim et.

Önce anlatımı oku: [`01_Anlatim.md`](01_Anlatim.md)

Bugünkü pratik, önceki günlerle birleşir:

| Gün | Ne öğrenmiştin? | Bugün nerede kullanacaksın? |
|-----|-----------------|------------------------------|
| Pazartesi | GPIO: LED yaz, buton oku | LED çıkış, pin Input + pull |
| Salı | Debounce, kısa / uzun basış | Spam yok, süre ölçümü |
| Çarşamba | Kesme, ISR, NVIC | Butonu poll etme; kenarda ISR |

---

## Ortak hazırlık

Başlamadan şunlar hazır olsun:

- [ ] Kart USB-C ile bağlı, proje derleniyor
- [ ] [`01_Anlatim.md`](01_Anlatim.md) okundu
- [ ] En az **1 LED** (çıkış) ve **kullanıcı butonu** (giriş) tanımlı
- [ ] Pin config'te buton için:
  - **Pin Mode:** Input
  - **Pull:** şemana göre Pull-Up veya Pull-Down
  - **Interrupt Operation Mode:** Edge
  - **Interrupt Triggering Mode:** Pull-up ise genelde **Falling**, Pull-down ise **Rising**
- [ ] İlgili kesme NVIC'te açık (enable)

---

## Kolay

**Amaç:** Butonu artık döngüde sürekli okumayacaksın. Basılınca donanım seni uyandıracak; sen de bir LED'i aç/kapa yapacaksın.

| | |
|---|---|
| **Görev** | Harici kesme ile LED toggle |
| **Yapıldı** | ☐ |

### Ne yapacaksın? (adım adım)

1. Buton pinini yukarıdaki gibi **kesme** olacak şekilde ayarla.
2. `volatile bool button_flag = false;` gibi bir bayrak tanımla (**MCU RAM**).
3. **ISR** içinde (kesme fonksiyonu):
   - `button_flag = true;` yaz
   - Kesme bayrağını / pending'i **temizle** (yoksa ISR tekrar tekrar girer)
   - Uzun iş, `delay`, LED yakma **yapma** — ISR kısa kalsın
4. `main` / `while(1)` içinde:
   - `button_flag` true ise → `false` yap → LED'i **toggle** et
5. Döngüde `Gpio_Read(buton)` ile sürekli kontrol **etme** (polling yasak).

### Doğru çalışıyor mu?

- Butona her (temiz) basışta LED bir kez değişir.
- Basılı tutunca LED çılgınca yanıp sönmez (en azından kolay seviyede kenar doğruysa).
- Polling kullandığını söylersen görev tamam sayılmaz.

### Kısa hatırlatma

```
butona bas → kenar → NVIC → ISR (flag=1) → main flag görür → LED toggle
```

---

## Orta

**Amaç:** Salı'daki kısa / uzun basışı, bugün **kesme** ile birleştir. ISR sadece "bir şey oldu" der; süre ve LED işi `main`'de kalır.

| | |
|---|---|
| **Görev** | Kesme + debounce + kısa / uzun basış |
| **Yapıldı** | ☐ |

### Ne yapacaksın? (adım adım)

1. Kolaydaki gibi buton **harici kesme** ile gelsin (`volatile` flag veya kenar bilgisi).
2. **Debounce** ekle — tek basış birden fazla ISR / event üretmesin:
   - yazılım: son geçerli zamandan beri örn. **20 ms** geçmediyse yok say, **veya**
   - pin config'te **Debouncing Filter** aç
3. Basılı tutma süresini ölç (`GetTickMs` / timer / bildiğin tick):
   - **Kısa basış** (örn. bırakınca süre < 1 sn) → **LED1** toggle
   - **Uzun basış** (örn. ≥ **1 sn**) → **LED2** kısa flash veya farklı pattern
4. Süre ölçümü ve LED animasyonu **ISR içinde olmasın**; `main` döngüsünde olsun.

### Doğru çalışıyor mu?

- Hızlı çift yanma / spam yok (debounce işliyor).
- Kısa basış ile uzun basış **farklı** LED davranışı üretiyor.
- ISR'da sadece flag / zaman damgası gibi hafif iş var.

### Salı ile fark

Salı'da butonu döngüde okuyordun. Bugün olay **EXTI / kesme** ile geliyor; kısa–uzun mantığı aynı, kaynak değişti.

---

## Zor

**Amaç:** Üç günün özetini tek projede topla: kayan ışık (Pazartesi) + debounce'lu event (Salı) + kesmeyle kontrol (Çarşamba). Sonra GitHub'a yükle.

| | |
|---|---|
| **Görev** | Kayan ışık + kesmeli kontrol + GitHub |
| **Yapıldı** | ☐ |

### Ne yapacaksın? (adım adım)

1. En az **3 LED** ile kayan ışık (chase): LED'ler sırayla aksın.
2. Ana döngü chase'i sürekli çalıştırsın.
3. Buton **kesme** ile gelsin; debounce'lu **tek event** üretsin (spam yok).
4. Event'e göre:
   - chase **yönünü** değiştir (ileri ↔ geri), **veya**
   - chase **hızını** değiştir (yavaş ↔ hızlı)
5. Tercihen kodu modüler hale getirin : `button.h` / `button.c` ve LED tarafı ayrı dosya; `main` init + döngü + event'e tepki.
6. Bitince kendi GitHub repona yükle (aşağıya bak).

### Doğru çalışıyor mu?

- Chase kendi başına akıyor.
- Butona basınca yön veya hız net değişiyor.
- ISR sadece sinyal; LED sırası `main`'de güncelleniyor.
- Repo linki raporda var.

### GitHub teslimi

1. Kendi staj reponu kullan (yoksa oluştur).
2. Bugünün klasörüne koy:

```
2026-07-22/
└── ...    (kaynak kod / proje)
```

3. Kısa README: ne yaptın, kenar Rising mi Falling mi, debounce nasıl.
4. Repo URL'sini `gunluk_rapor.md` içine yaz.

---

## Rapora eklenecek — Teorik Sorular

`gunluk_rapor.md` içinde **kendi cümlelerinle** cevapla. Hazır tanım kopyalama. Her soruya **2–5 cümle** yeterlidir. Mümkün olduğunca kendi yaptığın uygulamalardan örnek ver.

1. **GPIO** pinlerini **Input** ve **Output** modlarında karşılaştırınız. Bir LED ve bir buton örneği üzerinden çalışma mantığını açıklayınız.

2. **Pull-up**, **Pull-down** ve **Floating** giriş kavramlarını açıklayınız. Floating bir giriş pininin neden güvenilir olmadığını belirtiniz.

3. **MCU**, **MPU** ve **SoC** kavramlarını karşılaştırınız. Aralarındaki temel farkları, kullanım alanlarını ve kullandığınız A34G43x işlemcisinin hangi gruba girdiğini açıklayınız.

4. Bir gömülü yazılımda **global değişken**, **static değişken**, **yerel (local) değişken**, **fonksiyon** ve **string sabitlerinin** belleğin hangi bölgelerinde bulunduğunu açıklayınız.

5. **Polling** ve **Interrupt (Kesme)** yöntemlerini CPU kullanımı, tepki süresi ve enerji tüketimi açısından karşılaştırınız. Hangi durumlarda hangisinin tercih edilmesi daha uygundur?

6. **Interrupt Service Routine (ISR)** nedir? ISR içerisinde neden uzun süre çalışan işlemler, `delay` fonksiyonları veya yoğun `printf` kullanımı önerilmez?

7. **NVIC (Nested Vector Interrupt Controller)** ne işe yarar? Kesme önceliği (**priority**) ve iç içe kesme (**nested interrupt**) kavramlarını açıklayınız.

8. Mekanik butonlarda görülen **debounce (contact bounce)** problemi nedir? Yazılımsal ve donanımsal çözüm yöntemlerini karşılaştırınız.

9. Pull-up ile bağlanmış bir butonda neden genellikle **Falling Edge Interrupt** kullanılır? Rising Edge hangi durumlarda tercih edilebilir?

10. ISR ile `main()` fonksiyonu tarafından ortak kullanılan bir değişken neden **`volatile`** olarak tanımlanmalıdır? `volatile` kullanılmadığında derleyicinin yaptığı optimizasyonlar ne tür hatalara yol açabilir?


---

## Teslim

```
teslimler/Stajyer_X/
├── rapor/gunluk_rapor.md    ← teori cevapları + GitHub URL
└── proje/
```

Zor görev ayrıca: GitHub → `2026-07-22/`.
