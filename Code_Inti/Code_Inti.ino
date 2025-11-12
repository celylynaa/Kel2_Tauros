/*Kode Kelompok 2 TAUROS - ROBIN
------------------------------------------------------------------------------------*/
#include <ps5Controller.h>      // Library untuk menghubungkan ESP32 dengan controller PS5
#define MAX_MOTOR_SPEED 255     // Kecepatan maksimum motor (nilai PWM maksimum)

unsigned long lastTimeStamp = 0;  // Variabel untuk menyimpan waktu terakhir kirim data serial

// --- Deklarasi pin motor kanan (driver motor 1)
int enA = 25;   // Pin enable motor kanan (PWM)
int kPin1 = 16; // Pin arah motor kanan 1
int kPin2 = 17; // Pin arah motor kanan 2

// --- Deklarasi pin motor kiri (driver motor 2)
int enB = 23;   // Pin enable motor kiri (PWM)
int krPin1 = 18; // Pin arah motor kiri 1
int krPin2 = 19; // Pin arah motor kiri 2

// --- Pengaturan PWM
const int PWMFreq = 1000;                 // Frekuensi PWM = 1 kHz
const int PWMResolution = 8;              // Resolusi PWM = 8 bit (0–255)
const int rightMotorPWMSpeedChannel = 0;  // Kanal PWM motor kanan
const int leftMotorPWMSpeedChannel = 1;   // Kanal PWM motor kiri

// Callback ketika ada input dari PS5
void notify() {
  char messageString[200];
  sprintf(messageString, "%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d",
          ps5.Up(), ps5.Down(), ps5.Right(), ps5.Left(),
          ps5.Square(), ps5.Cross(), ps5.Circle(), ps5.Triangle());

  // Menampilkan status tombol ke serial monitor setiap 100ms
  if (millis() - lastTimeStamp > 100) {
    Serial.println(messageString);
    lastTimeStamp = millis();
  }
}

// Callback ketika PS5 terkoneksi
void onConnect() {
  Serial.println("PS5 Connected!");
}

// Callback ketika PS5 terputus
void onDisConnect() {
  Serial.println("PS5 Disconnected!");
}

// Fungsi untuk mengatur arah dan kecepatan motor kanan (kanan) dan kiri (kiri)
void rotate(int kanan, int kiri) {
  // --- Motor Kanan (pin kPin1, kPin2)
  if (kanan < 0) {  // Mundur
    digitalWrite(kPin1, LOW);
    digitalWrite(kPin2, HIGH);
  } else if (kanan > 0) {  // Maju
    digitalWrite(kPin1, HIGH);
    digitalWrite(kPin2, LOW);
  } else {  // Berhenti
    digitalWrite(kPin1, LOW);
    digitalWrite(kPin2, LOW);
  }

  // --- Motor kiri (pin krPin1, krPin2)
  if (kiri < 0) {  // Mundur
    digitalWrite(krPin1, LOW);
    digitalWrite(krPin2, HIGH);
  } else if (kiri > 0) {  // Maju
    digitalWrite(krPin1, HIGH);
    digitalWrite(krPin2, LOW);
  } else {  // Berhenti
    digitalWrite(krPin1, LOW);
    digitalWrite(krPin2, LOW);
  }

  // Atur kecepatan motor (nilai absolut PWM)
  ledcWrite(rightMotorPWMSpeedChannel, abs(kanan));
  ledcWrite(leftMotorPWMSpeedChannel, abs(kiri));
}

// --- Fungsi gerakan dasar robot ---
void maju () {
  rotate(MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);   // Kedua motor maju
}
void mundur() {
  rotate(-MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED); // Kedua motor mundur
}
void belKir() {
  rotate(MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);  // Belok kiri
}
void belKan() {
  rotate(-MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);  // Belok kanan
}
void berhenti() {
  rotate(0, 0);                               // Stop semua motor
}

// --- Setup awal ---
void setup() {
  Serial.begin(115200);             // Inisialisasi komunikasi serial
  ps5.attach(notify);               // Pasang callback saat tombol ditekan
  ps5.attachOnConnect(onConnect);   // Callback saat terkoneksi
  ps5.attachOnDisconnect(onDisConnect); // Callback saat terputus
  
  ps5.begin("48:18:8D:8F:B1:DE");   // Alamat MAC Bluetooth dari ESP32

  // Tunggu sampai PS5 terkoneksi
  while (!ps5.isConnected()) {
    Serial.println("mana ya PS5 nya...");
    delay(500);
  }

  Serial.println("PS5 siap!");

  // Set semua pin motor sebagai output
  pinMode(enA, OUTPUT);
  pinMode(kPin1, OUTPUT);
  pinMode(kPin2, OUTPUT);
  
  pinMode(enB, OUTPUT);
  pinMode(krPin1, OUTPUT);
  pinMode(krPin2, OUTPUT);

  // Inisialisasi PWM untuk masing-masing motor
  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(enA, rightMotorPWMSpeedChannel);
  ledcAttachPin(enB, leftMotorPWMSpeedChannel);

  berhenti();  // Pastikan motor dalam keadaan berhenti di awal
}

// --- Loop utama ---
void loop() {
  // Jika PS5 tidak terhubung, hentikan motor
  if (!ps5.isConnected()) {
    berhenti();
    return;
  }

  // Cek tombol dan jalankan perintah sesuai tombol PS5
  if (ps5.Triangle()) {
    maju();      // Tombol segitiga = maju
  } else if (ps5.Cross()) {
    mundur();        // Tombol silang = mundur
  } else if (ps5.Square()) {
    belKir();      // Tombol kotak = belok kiri
  } else if (ps5.Circle()) {
    belKan();      // Tombol lingkaran = belok kanan
  } else {
    berhenti();    // Tidak ada tombol ditekan = berhenti
  }

  delay(100);  // Delay kecil agar perintah tidak terlalu cepat berubah
}

