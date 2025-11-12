/*Kode Kelompok 2 TAUROS - ROBIN
------------------------------------------------------------------------------------*/
// Library untuk menghubungkan ESP32 dengan controller PS5 via Bluetooth
#include <ps5Controller.h>

// Kecepatan maksimum motor (versi 1)
#define MAX_MOTOR_SPEED 100
// Kecepatan maksimum motor (versi 2, lebih cepat)
#define max_motor_speed 255

// Variabel untuk mencatat waktu terakhir update data PS5
unsigned long lastTimeStamp = 0;

// --- Konfigurasi pin motor kanan ---
int enA = 25; // pin enable (PWM) untuk motor kanan (driver motor1)
int kPin1 = 16; // pin arah 1 motor kanan
int kPin2 = 17; // pin arah 2 motor kanan

// --- Konfigurasi pin motor kiri ---
int enB = 23; // pin enable (PWM) untuk motor kiri (driver motor2)
int krPin1 = 18; // pin arah 1 motor kiri
int krPin2 = 19; // pin arah 2 motor kiri

// --- Konfigurasi PWM ---
const int PWMFreq = 1000;               // frekuensi PWM = 1 kHz
const int PWMResolution = 8;            // resolusi PWM = 8-bit (0–255)
const int rightMotorPWMSpeedChannel = 0; // channel PWM motor kanan
const int leftMotorPWMSpeedChannel = 1;  // channel PWM motor kiri

// Callback saat ada data masuk dari PS5 controller
void notify() {
  char messageString[200];
  // Format string untuk menampilkan status tombol (1=tekan, 0=tidak)
  sprintf(messageString, "%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d",
          ps5.Up(), ps5.Down(), ps5.Right(), ps5.Left(),
          ps5.Square(), ps5.Cross(), ps5.Circle(), ps5.Triangle());

  // Hanya cetak tiap 100ms agar serial tidak penuh
  if (millis() - lastTimeStamp > 100) {
    Serial.println(messageString);
    lastTimeStamp = millis();
  }
}

// Callback saat PS5 berhasil terhubung
void onConnect() {
  Serial.println("PS5 Connected!");
}

// Callback saat PS5 terputus
void onDisConnect() {
  Serial.println("PS5 Disconnected!");
}

// Fungsi untuk mengatur arah dan kecepatan motor kanan & kiri
void rotate(int kanan, int kiri) {
  // --- Kontrol motor kanan ---
  if (kanan < 0) {
    digitalWrite(kPin1, LOW);
    digitalWrite(kPin2, HIGH);
  } else if (kanan > 0) {
    digitalWrite(kPin1, HIGH);
    digitalWrite(kPin2, LOW);
  } else {
    digitalWrite(kPin1, LOW);
    digitalWrite(kPin2, LOW);
  }

  // --- Kontrol motor kiri---
  if (kiri < 0) {
    digitalWrite(krPin1, LOW);
    digitalWrite(krPin2, HIGH);
  } else if (kiri > 0) {
    digitalWrite(krPin1, HIGH);
    digitalWrite(krPin2, LOW);
  } else {
    digitalWrite(krPin1, LOW);
    digitalWrite(krPin2, LOW);
  }

  // Mengatur kecepatan PWM motor
  ledcWrite(rightMotorPWMSpeedChannel, abs(kanan));
  ledcWrite(leftMotorPWMSpeedChannel, abs(kiri));
}

// --- Fungsi pergerakan dasar ---
void maju () {
  rotate(MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
}

void mundur() {
  rotate(-MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
}

void belKir() {
  rotate(MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
}

void belKan() {
  rotate(-MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
}

// --- Fungsi pergerakan versi cepat (pakai kecepatan 255) ---
void maju1 () {
  rotate(max_motor_speed, max_motor_speed);
}

void mundur1() {
  rotate(-max_motor_speed, -max_motor_speed);
}

void belKir1() {
  rotate(max_motor_speed, -max_motor_speed);
}

void belKan1() {
  rotate(-max_motor_speed, max_motor_speed);
}

// --- Fungsi berhenti ---
void berhenti() {
  rotate(0, 0);
}

// --- Setup awal (dijalankan sekali saat boot) ---
void setup() {
  Serial.begin(115200); // buka komunikasi serial
  // Pasang callback fungsi
  ps5.attach(notify);
  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisConnect);
  
  // Ganti MAC address di sini sesuai alamat PS5 (tanpa spasi)
  ps5.begin("48:18:8D:8F:B1:DE"); 

  // Tunggu sampai PS5 benar-benar connect
  while (!ps5.isConnected()) {
    Serial.println("mana ya PS5 nya...");
    delay(500);
  }

  Serial.println("PS5 siap!");

  // Set pin motor sebagai output
  pinMode(enA, OUTPUT);
  pinMode(kPin1, OUTPUT);
  pinMode(kPin2, OUTPUT);
  
  pinMode(enB, OUTPUT);
  pinMode(krPin1, OUTPUT);
  pinMode(krPin2, OUTPUT);

  // Setup PWM untuk kedua motor
  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);

  // Menghubungkan pin motor ke channel PWM
  ledcAttachPin(enA, rightMotorPWMSpeedChannel); 
  ledcAttachPin(enB, leftMotorPWMSpeedChannel);  

  berhenti(); // pastikan motor mati di awal
}

// --- Loop utama ---
void loop() {
  // Jika PS5 tidak terkoneksi, berhenti dan keluar loop
  if (!ps5.isConnected()) {
    berhenti();
    return;
  }

  // Kontrol gerakan berdasarkan tombol yang ditekan
  if (ps5.Up()){
    maju();     
  } else if (ps5.Down()){
    mundur();      
  } else if (ps5.Left()){
    belKir();    
  } else if (ps5.Right()) {
    belKan();  
  } else if (ps5.Triangle()){
    mundur1();    // versi cepat mundur
  } else if (ps5.Cross()){
    maju1();      // versi cepat maju
  } else if (ps5.Square()){
    belKir1();    // versi cepat belok kiri
  } else if (ps5.Circle()) {
    belKan1();    // versi cepat belok kanan
  } else {
    berhenti();   // kalau tidak ada tombol ditekan, berhenti
  }

  delay(100); // jeda kecil supaya tidak terlalu cepat loop-nya
}
