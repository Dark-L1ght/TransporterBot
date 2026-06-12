# Transporter Bot

**Dokumentasi Lengkap Proyek Robot Dua Mikrokontroler (ESP32 & Arduino Nano)**

Proyek ini adalah sistem robot transporter yang dikendalikan melalui Web Interface (via WiFi). Sistem ini menggunakan arsitektur *dual-microcontroller* untuk memisahkan beban kerja:
1.  **ESP32**: Bertindak sebagai "Otak" dan Antarmuka. Menangani WiFi, Web Server, dan menerima input dari pengguna.
2.  **Arduino Nano**: Bertindak sebagai "Otot" atau Driver. Menerima perintah dari ESP32 dan menggerakkan motor serta servo secara langsung.

---

## 🏗️ Arsitektur Sistem

Modul komunikasi yang digunakan adalah **Serial UART**. Berikut alur datanya:

`[User / HP]` --(WiFi)--> `[ESP32]` --(Serial TX-RX)--> `[Arduino Nano]` --(GPIO/PWM)--> `[Motor & Servo]`

*   **ESP32** membuat Hotspot WiFi (`Robot_Controller`).
*   Pengguna mengakses IP `192.168.4.1` untuk membuka Web Controller.
*   Saat tombol ditekan, ESP32 mengirim karakter perintah (misal: 'F' untuk maju) ke Arduino Nano.
*   Arduino Nano membaca karakter tersebut dan mengeksekusi gerakan motor atau servo.

### Komunikasi Bluetooth (Opsional)
Nano juga dapat terhubung ke modul **HC-05 Bluetooth** untuk debugging via serial. Komunikasi ini menggunakan SoftwareSerial pada pin **D11 (RX)** dan **D12 (TX)** — bebas konflik dengan hardware Serial yang digunakan untuk komunikasi ESP32.

---

## 📁 Struktur File

| File | Deskripsi |
|------|-----------|
| `Esp32.ino` | Firmware ESP32 — WiFi AP, Web Server, handler perintah |
| `index.h` | Halaman Web GUI (HTML + CSS + JavaScript) — dipisahkan dari Esp32.ino |
| `Nano.ino` | Firmware Arduino Nano — driver motor L298N, servo gripper & lift |
| `.clangd` | Konfigurasi clangd (lokal, tidak di-commit) |
| `.gitignore` | Daftar file yang diabaikan git |

---

## 🔌 Persiapan Hardware & Wiring

### 1. Komponen Utama
*   1x ESP32 Development Board (DOIT DEVKIT V1 atau sejenisnya)
*   1x Arduino Nano
*   1x Modul Driver Motor L298N (atau sejenis)
*   3x Servo Motor (Untuk Gripper Kiri, Kanan, dan Lift)
*   1x Modul Bluetooth HC-05 (opsional, untuk debugging)
*   Sumber Daya (Baterai Li-Ion/Li-Po disarankan untuk Motor dan Servo)

### 2. Wiring ESP32
| Pin ESP32 | Terhubung Ke | Fungsi |
| :--- | :--- | :--- |
| **GND** | GND (Common) | Ground (Wajib disatukan dengan Nano & Baterai) |
| **VIN/5V** | 5V Power | Input Daya |
| **GPIO 16 (RX2)** | TX Arduino Nano (D1) | Menerima data dari Nano (jika ada) |
| **GPIO 17 (TX2)** | RX Arduino Nano (D0) | Mengirim perintah ke Nano |

> **⚠️ PENTING:** Pastikan **GND** (Ground) semua komponen (ESP32, Nano, Driver Motor, Baterai) terhubung menjadi satu (Common Ground). Jika tidak, komunikasi Serial tidak akan bekerja.

### 3. Wiring Arduino Nano
#### Koneksi ke Motor Driver (L298N)
| Pin Nano | Pin Driver Motor | Fungsi |
| :--- | :--- | :--- |
| **D2** | ENA | PWM Speed Motor Kiri |
| **D3** | IN1 | Arah Motor Kiri A |
| **D4** | IN2 | Arah Motor Kiri B |
| **D5** | IN3 | Arah Motor Kanan A |
| **D6** | IN4 | Arah Motor Kanan B |
| **D7** | ENB | PWM Speed Motor Kanan |

#### Koneksi ke Servo
| Pin Nano | Servo | Fungsi |
| :--- | :--- | :--- |
| **D8** | Servo Kanan | Mencapit (Gripper Right) |
| **D9** | Servo Kiri | Mencapit (Gripper Left) |
| **D10** | Servo Lift | Mengangkat (Lift Up/Down) |

#### Koneksi Komunikasi (Serial)
| Pin Nano | Terhubung Ke | Catatan |
| :--- | :--- | :--- |
| **RX (D0)** | GPIO 17 (TX) ESP32 | Menerima perintah dari ESP32 |
| **TX (D1)** | GPIO 16 (RX) ESP32 | Mengirim status (opsional) |

#### Koneksi Bluetooth HC-05 (Opsional)
| Pin Nano | Pin HC-05 | Catatan |
| :--- | :--- | :--- |
| **D11** | TX | Menerima data dari HC-05 (RX SoftwareSerial) |
| **D12** | RX | Mengirim data ke HC-05 (TX SoftwareSerial) |

> **⚠️ PERUBAHAN dari versi sebelumnya:** Bluetooth sekarang menggunakan **D11/D12**, bukan D0/D1. Ini menghindari konflik dengan hardware Serial (USB) yang digunakan untuk upload program dan komunikasi ESP32.

---

## 🚀 Panduan Instalasi & Upload

### Langkah 1: Upload ke ESP32
1.  Buka file `Esp32.ino` di Arduino IDE (file `index.h` akan terbuka otomatis di tab terpisah).
2.  Pilih Board **DOIT ESP32 DEVKIT V1** di Arduino IDE.
3.  Pastikan library `WiFi` dan `WebServer` sudah terinstall (bawaan paket ESP32).
4.  Hubungkan ESP32 ke PC, lalu **Upload**.

### Langkah 2: Upload ke Arduino Nano
1.  Buka file `Nano.ino` di Arduino IDE.
2.  Pilih Board **Arduino Nano** di Arduino IDE.
3.  Pastikan library `Servo` dan `SoftwareSerial` sudah terinstall.
4.  Hubungkan Nano ke PC, lalu **Upload**.

> **Catatan:** Tidak perlu mencabut kabel RX/TX saat upload — Bluetooth sekarang ada di pin D11/D12, bukan D0/D1.

---

## 🎮 Cara Penggunaan

1.  Nyalakan Robot (Pastikan baterai terpasang).
2.  Di HP/Laptop, cari WiFi bernama:
    *   **SSID**: `Robot_Controller`
    *   **Password**: `12345678`
3.  Buka Browser (Chrome/Safari), ketik alamat: **`192.168.4.1`**
4.  Akan muncul tampilan **Controller**.
    *   **Landscape Mode**: Putar HP ke posisi landscape agar tombol terlihat rapi.
    *   Tampilan full screen disarankan (klik ikon pojok kanan atas).
5.  Gunakan tombol arah untuk menggerakkan robot dan tombol fungsi untuk menggerakkan Gripper/Lift.

---

## 🧠 Penjelasan Teknis & "How It Works"

### Protokol Komunikasi
Sistem menggunakan protokol satu karakter sederhana (`char`) untuk efisiensi.
Berikut kamus perintah yang dikirim dari ESP32 ke Nano:

| Karakter | Fungsi | Penjelasan |
| :---: | :--- | :--- |
| **'F'** | Maju (Forward) | Kedua motor maju |
| **'B'** | Mundur (Backward) | Kedua motor mundur |
| **'L'** | Kiri (Left) | Motor kiri mundur, kanan maju (Pivot) |
| **'R'** | Kanan (Right) | Motor kiri maju, kanan mundur (Pivot) |
| **'P'** | Stop (Pause) | Semua motor berhenti |
| **'T'** | Lift UP (Triangle) | Servo Lift naik (sudut 180) |
| **'X'** | Lift DOWN (Cross) | Servo Lift turun (sudut 95) |
| **'C'** | Gripper OPEN (Circle)| Membuka capit |
| **'S'** | Gripper CLOSE (Square)| Menutup capit |
| **'A'** | Kalibrasi (Start) | Set posisi servo ke default/nol |
| **'V'** | Speed | Prefix untuk set kecepatan (diikuti angka 0–255) |

### Logika Program
1.  **ESP32 (`Esp32.ino` + `index.h`)**:
    *   Halaman HTML terpisah di file `index.h` — lebih bersih dan mudah diedit.
    *   Menggunakan AJAX (`XMLHttpRequest`) di JavaScript untuk mengirim perintah tanpa me-refresh halaman.
    *   Fungsi `handleCommand()` menangkap request `/cmd?val=X` dan meneruskannya ke Serial2 (`Serial2.print`).
    *   Fungsi `handleSpeed()` memvalidasi nilai slider (0–255) sebelum dikirim.

2.  **Arduino Nano (`Nano.ino`)**:
    *   Loop utama (`void loop`) memantau `Serial.available()`.
    *   Jika ada data masuk, data dibaca dan diproses di `processBluetoothCommand()`.
    *   Berdasarkan karakter yang diterima, fungsi `motorCtrl` atau `gripperCtrl` dipanggil untuk mengubah state pin digital/PWM.
    *   Kecepatan motor disimpan di variabel `currentSpeed` — perubahan dari slider akan tersimpan dan digunakan untuk gerakan selanjutnya.

### Fitur Web Interface
*   **Touch-optimized**: Anti-zoom, anti-scroll, anti-select — robot control panel yang responsif.
*   **Landscape-only**: Peringatan jika HP dalam posisi portrait.
*   **Fullscreen**: Tombol fullscreen di pojok kanan atas.
*   **Tombol tekan-tahan**: `ontouchstart` untuk gerak, `ontouchend` untuk stop — robot berhenti saat jari diangkat.

---

## 🛠️ Panduan Modifikasi (Customization)

### 1. Mengubah Pin Motor & Servo (Pada Nano)
Jika Anda menggunakan kaki/pin yang berbeda di Arduino Nano, ubah bagian ini di `Nano.ino`:

```cpp
struct MotorConfig {
    // Ubah angka pin di sini sesuai wiring Anda
    const uint8_t ENA = 2;
    const uint8_t IN1 = 3;
    ...
};

struct GripperConfig {
    // Ubah pin servo di sini
    const uint8_t GRIPPER_RIGHT_PIN = 8;
    ...
};
```

### 2. Kalibrasi Sudut Servo (Pada Nano)
Jika servo Anda bergerak terlalu jauh atau mentok, ubah nilai sudutnya di `Nano.ino`:

```cpp
struct GripperConfig {
    // Sesuaikan sudut (range 0 - 180)
    const uint8_t GRIPPER_LEFT_OPEN = 20;
    const uint8_t GRIPPER_LEFT_CLOSE = 51;
    const uint8_t LIFT_UP = 180;            
    const uint8_t LIFT_DOWN = 95;           
    ...
};
```

### 3. Mengubah Tampilan Web Interface
Tampilan web ada di file **`index.h`** — HTML/CSS/JS murni.
*   **Mengganti Warna**: Cari bagian style CSS di `index.h`, misal `.btn-move` untuk tombol gerak.
*   **Mengubah Posisi Tombol**: Edit bagian Grid CSS (`grid-column`, `grid-row`).
*   **Menambah Tombol Baru**: Tambahkan button di HTML + handler di JavaScript, lalu tambahkan case di `Esp32.ino` → `Nano.ino`.

### 4. Mengubah Kecepatan Default (Pada Nano)
Kecepatan motor diatur via PWM (0-255). Defaultnya ada di struktur motor `Nano.ino`:

```cpp
const uint8_t DEFAULT_SPEED = 140; // Ganti nilai ini (0-255)
```

### 5. Pin Bluetooth (Pada Nano)
Jika Anda ingin mengganti pin Bluetooth HC-05, ubah di `Nano.ino`:

```cpp
SoftwareSerial bluetooth(11, 12); // Ganti angka pin di sini
```

---

## 📋 Changelog

### v2.0 — Perbaikan Kode

| Perbaikan | Detail |
|-----------|--------|
| **Pin Bluetooth fix** | Pindah dari D0/D1 (konflik hardware Serial) ke D11/D12 |
| **Speed slider bug fix** | Slider sekarang menyimpan nilai ke `currentSpeed` — gerakan selanjutnya pakai kecepatan baru |
| **Buffer flush fix** | Ganti `while(Serial.available())` agresif dengan flush terbatas — tidak ada lagi perintah termakan |
| **Timeout protection** | `Serial.setTimeout(50)` mencegah `parseInt()` blocking 1 detik |
| **Servo init sekali** | Servo di-attach di `setup()` saja — tidak setiap kali gerak |
| **Dead code removal** | Hapus `processSerialCommand()` yang tidak pernah dipanggil |
| **HTML dipisah** | GUI web pindah ke `index.h` — file `Esp32.ino` sekarang hanya ~70 baris logika |
| **Speed validation** | Nilai slider 0–255 divalidasi di ESP32 sebelum dikirim |
| **Naming consistent** | `CalibrateArmGripper()` → `calibrateGripper()` |

---

## ❓ Troubleshooting (Masalah Umum)

**Q: Robot tidak bergerak saat tombol ditekan di Web.**
*   Cek apakah ESP32 dan Nano sudah terhubung *Common Ground* (GND disatukan).
*   Pastikan kabel RX ESP32 (GPIO16) ke TX Nano (D1), dan TX ESP32 (GPIO17) ke RX Nano (D0) — **silang**.
*   Cek apakah Driver Motor saklar power-nya sudah ON.
*   Cek Serial Monitor ESP32 (115200 baud) — apakah ada log "Cmd: X" saat tombol ditekan?

**Q: Web interface tidak bisa dibuka.**
*   Pastikan Anda sudah connect ke WiFi `Robot_Controller`.
*   Matikan paket data (Mobile Data) di HP, karena kadang HP menolak koneksi WiFi tanpa internet.
*   Cek Serial Monitor ESP32 — apakah IP Address muncul (`192.168.4.1`)?

**Q: Bluetooth tidak terdeteksi / error.**
*   Pastikan pin HC-05 terhubung ke **D11 (Nano RX → HC-05 TX)** dan **D12 (Nano TX → HC-05 RX)**.
*   Jangan gunakan pin D0/D1 — sekarang dipakai untuk komunikasi dengan ESP32.

**Q: Tidak bisa upload program ke Nano.**
*   Tidak perlu cabut kabel D0/D1 lagi karena Bluetooth sudah di pin D11/D12.
*   Namun pastikan tidak ada device lain yang mengirim data ke Serial Nano saat upload.

**Q: Ada file `nul` aneh di git.**
*   Itu adalah phantom file Windows (reserved device name). Sudah di-ignore di `.gitignore`. Aman untuk diabaikan.
