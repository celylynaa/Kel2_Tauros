#include <ps5Controller.h>
#define MAX_MOTOR_SPEED 255

unsigned long lastTimeStamp = 0;

int enA = 5; //driver motor1
int kPin1 = 16;
int kPin2 = 17;

int enB = 23; //driver motor2
int krPin1 = 18;
int krPin2 = 19;

const int PWMFreq = 1000;
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

void notify() {
  char messageString[200];
  sprintf(messageString, "%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d",
          ps5.Up(), ps5.Down(), ps5.Right(), ps5.Left(),
          ps5.Square(), ps5.Cross(), ps5.Circle(), ps5.Triangle());

  if (millis() - lastTimeStamp > 100) {
    Serial.println(messageString);
    lastTimeStamp = millis();
  }
}

void onConnect() {
  Serial.println("PS5 Connected!");
}

void onDisConnect() {
  Serial.println("PS5 Disconnected!");
}

void rotate(int kanan, int kiri) {
  // motor kirinyaa
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

  //motor kanann nyaa
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
  ledcWrite(rightMotorPWMSpeedChannel, abs(kanan));
  ledcWrite(leftMotorPWMSpeedChannel, abs(kiri));
}

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
  ps5.attach(notify);
  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisConnect);
  
  ps5.begin("48:18:8D:8F:B1:DE "); 

  while (!ps5.isConnected()) {
    Serial.println("mana ya PS5 nya...");
    delay(500);
  }

  Serial.println("PS5 siap!");

  pinMode(enA, OUTPUT);
  pinMode(kPin1, OUTPUT);
  pinMode(kPin2, OUTPUT);
  
  pinMode(enB, OUTPUT);
  pinMode(krPin1, OUTPUT);
  pinMode(krPin2, OUTPUT);

  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(enA, rightMotorPWMSpeedChannel);
  ledcAttachPin(enB, leftMotorPWMSpeedChannel);

  berhenti();
}

void loop() {
  if (!ps5.isConnected()) {
    berhenti();
    return;
  }

  if (ps5.Up()){
    belKir();
  } else if (ps5.Down()){
    belKan();
  } else if (ps5.Left()){
    maju();
  } else if (ps5.Right()) {
    mundur();
  } else {
    berhenti();
  }
  delay(100);
}
















