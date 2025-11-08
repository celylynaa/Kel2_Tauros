/*Kode Kelompok 2 TAUROS - ROBIN
------------------------------------------------------------------------------------*/

#include <ps5Controller.h>   // Library komunikasi controller PS5 ke ESP32
#define MAX_MOTOR_SPEED 255  // Kecepatan maksimum motor (PWM 0–255)

unsigned long lastTimeStamp = 0;

// Pin driver motor kanan
int enA = 5;    // Pin PWM motor kanan
int kPin1 = 16; // Pin arah motor kanan
int kPin2 = 17; // Pin arah motor kanan

// Pin driver motor kiri
int enB = 23;   // Pin PWM motor kiri
int krPin1 = 18; // Pin arah motor kiri
int krPin2 = 19; // Pin arah motor kiri

// Pengaturan PWM
const int PWMFreq = 1000;
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

void notify() {
  // Menampilkan tombol PS5 yang ditekan ke Serial Monitor
  char messageString[200];
  sprintf(messageString, "%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d",
          ps5.Up(), ps5.Down(), ps5.Right(), ps5.Left(),
          ps5.Square(), ps5.Cross(), ps5.Circle(), ps5.Triangle());

  if (millis() - lastTimeStamp > 100) {
    Serial.println(messageString);
    lastTimeStamp = millis();
  }
}

// Event ketika controller tersambung
void onConnect() {
  Serial.println("PS5 Connected!");
}

// Event ketika controller terputus
void onDisConnect() {
  Serial.println("PS5 Disconnected!");
}

// Fungsi kontrol motor (kanan, kiri)
void rotate(int kanan, int kiri) {
  // Kontrol arah motor kiri
  if (kanan < 0) {             // Mundur
    digitalWrite(kPin1, LOW);
    digitalWrite(kPin2, HIGH);
  } else if (kanan > 0) {      // Maju
    digitalWrite(kPin1, HIGH);
    digitalWrite(kPin2, LOW);
  } else {                     // Berhenti
    digitalWrite(kPin1, LOW);
    digitalWrite(kPin2, LOW);
  }

  // Kontrol arah motor kanan
  if (kiri < 0) {              // Mundur
    digitalWrite(krPin1, LOW);
    digitalWrite(krPin2, HIGH);
  } else if (kiri > 0) {       // Maju
    digitalWrite(krPin1, HIGH);
    digitalWrite(krPin2, LOW);
  } else {                     // Berhenti
    digitalWrite(krPin1, LOW);
    digitalWrite(krPin2, LOW);
  }

  // Menulis nilai PWM untuk kecepatan motor
  ledcWrite(rightMotorPWMSpeedChannel, abs(kanan));
  ledcWrite(leftMotorPWMSpeedChannel, abs(kiri));
}

// Fungsi gerakan robot
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

void berhenti() {
  rotate(0, 0);
}

void setup() {
  Serial.begin(115200);

  // Menghubungkan callback PS5
  ps5.attach(notify);
  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisConnect);
  
  ps5.begin("48:18:8D:8F:B1:DE "); // Alamat MAC ESP32 untuk Pair PS5 Controller

  // Tunggu hingga controller tersambung
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

  // Setup PWM
  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(enA, rightMotorPWMSpeedChannel);
  ledcAttachPin(enB, leftMotorPWMSpeedChannel);

  berhenti(); // Awal kondisi diam
}

void loop() {
  // Jika controller terputus, motor berhenti
  if (!ps5.isConnected()) {
    berhenti();
    return;
  }

  // Kendali robot berdasarkan tombol d-pad PS5
  if (ps5.Up()||ps5.Triangle()){
    maju();     // Tombol atas kiri atau tombol atas kanan (Segitiga) → maju
  } else if (ps5.Down()||ps5.Cross()){
    mundur();     // Tombol bawah kiri atau tombol bawah kanan (X) → mundur
  } else if (ps5.Left()){
    belKir();       // Tombol kiri → kiri
  } else if (ps5.Circle()) {
    belKan();     // Tombol kanan (lingkaran o) → kanan
  } else {
    berhenti();   // Tidak ada input → berhenti
  }

  delay(100); // Delay untuk stabilisasi
}