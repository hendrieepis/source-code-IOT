# wsn-udp: Kontrol LED ESP32 lewat UDP

Project sederhana untuk menguji UDP di ESP32. ESP32 tersambung ke WiFi dan mendengarkan UDP di port **4210**:

| Kirim | Hasil | Balasan |
|---|---|---|
| `1` | LED di GPIO23 menyala | `LED ON` |
| `0` | LED di GPIO23 mati | `LED OFF` |
| lainnya | tidak ada perubahan | `Kirim 1 atau 0` |

## Kebutuhan

- Board ESP32 (`esp32dev`), terhubung ke **COM33**
- LED + resistor 220–330 Ω dari **GPIO23** ke GND
- [PlatformIO Core](https://docs.platformio.org/page/core/installation.html) (`pio`)
- Ncat (netcat dari paket Nmap)

## 1. Install Ncat pakai winget

Ncat ikut terpasang bersama Nmap. Di PowerShell:

```powershell
winget install -e --id Insecure.Nmap
```

Tutup lalu buka lagi PowerShell, kemudian cek:

```powershell
ncat --version
```

Kalau `ncat` tidak dikenali, tambahkan `C:\Program Files (x86)\Nmap` ke PATH.

## 2. Buat project dengan `pio project init`

```powershell
cd C:\Users\<user>\Documents\PlatformIO
pio project init -d wsn-udp --board esp32dev
cd wsn-udp
```

> Kalau prompt diawali `(base)`, jalankan `conda deactivate` dulu. Python dari Anaconda bisa bentrok dengan PlatformIO.

## 3. Atur `platformio.ini`

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_port = COM33
monitor_port = COM33
```

Kalau port ESP32 berbeda, cek dengan `pio device list`.

## 4. Isi kode `src/main.cpp`

Pengaturan di atas file. Bawaannya memakai **hotspot smartphone** dengan SSID `your_ssid` dan password `your_password`. Cara ini sudah dicoba dan berhasil, dan paling gampang karena tidak terhalang aturan jaringan kantor atau kampus.

Cara pakai hotspot smartphone:
1. Di HP, aktifkan hotspot dengan nama `your_ssid` dan password `your_password`. Pilih pita **2.4 GHz** kalau ada pilihannya.
2. Sambungkan laptop ke hotspot yang sama.
3. Upload kode ke ESP32. ESP32 akan otomatis tersambung ke hotspot itu.

Kalau mau pakai WiFi atau nama hotspot lain, ganti dua baris ini (laptop juga harus tersambung ke WiFi yang sama, lihat langkah 7):

```cpp
const char* WIFI_SSID = "your_ssid";      // nama WiFi
const char* WIFI_PASS = "your_password";  // password WiFi

const uint16_t UDP_PORT = 4210;
const int LED_PIN = 23;
```

Cara kerja `loop()`: ESP32 membaca paket UDP, mengambil karakter pertama (spasi dan Enter diabaikan), menyalakan atau mematikan LED, lalu membalas ke pengirim.

## 5. Build dan upload

```powershell
pio run              # build
pio run -t upload    # upload ke COM33
```

Build pertama agak lama karena PlatformIO mengunduh compiler dan framework ESP32.

## 6. Cek koneksi pakai serial monitor

```powershell
pio device monitor
```

Kalau berhasil, akan muncul:

```
Menghubungkan WiFi...........
IP ESP32: 192.168.1.xxx
UDP listen di port 4210
```

Catat **IP ESP32**. Tekan tombol **EN/RST** di board kalau tulisan itu tidak muncul. Keluar dengan `Ctrl+C`.

> Kalau titik-titik `....` terus muncul tanpa henti, SSID atau password salah, atau WiFi-nya 5 GHz (ESP32 hanya mendukung 2.4 GHz).

## 7. Pastikan laptop dan ESP32 di jaringan yang sama

Ini syarat wajib. Kalau berbeda, paket UDP tidak akan sampai.

Cek IP dan WiFi laptop:

```powershell
ipconfig
netsh wlan show interfaces
```

- SSID laptop harus **sama** dengan `WIFI_SSID` di `main.cpp`.
- Tiga angka depan IP harus sama, misalnya laptop `192.168.1.199` dan ESP32 `192.168.1.xxx`.

Lalu tes ping:

```powershell
ping 192.168.1.xxx
```

Kalau ping gagal padahal WiFi-nya sudah sama, kemungkinan router mengaktifkan *client/AP isolation* (umum di WiFi kantor atau kampus). Solusinya, pakai hotspot smartphone seperti di langkah 4.

> Dengan hotspot smartphone, IP-nya biasanya tidak berbentuk `192.168.1.x` (misalnya `10.x.x.x` atau `192.168.43.x`). Itu normal. Yang penting laptop dan ESP32 tersambung ke hotspot yang sama.

## 8. Test LED pakai Ncat

Biarkan `pio device monitor` tetap terbuka di satu terminal. Di terminal lain:

```powershell
ncat -u 192.168.1.xxx 4210
```

Ketik lalu tekan Enter:

```
1
LED ON
0
LED OFF
```

Di serial monitor akan muncul:

```
Dari 192.168.1.199:54321 -> "1"
Dari 192.168.1.199:54321 -> "0"
```

Keluar dari ncat dengan `Ctrl+C`.

## Troubleshooting

| Gejala | Penyebab / solusi |
|---|---|
| Tidak ada baris `Dari ...` di monitor | Paket tidak sampai: laptop dan ESP32 beda jaringan (langkah 7) |
| `Dari ...` muncul tapi LED tidak menyala | Cek `LED_PIN`, arah kaki LED, dan resistor |
| Upload gagal / port sibuk | Tutup `pio device monitor` sebelum upload |
| `ncat` tidak dikenali | Buka ulang terminal atau tambahkan Nmap ke PATH |
| IP ESP32 berubah | Lihat lagi di serial monitor (IP dari DHCP bisa berubah setelah restart router) |
