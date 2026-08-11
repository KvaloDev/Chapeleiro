#include "baseP.h"

void setup() {
  Serial.begin(115200);

  initRobot();

  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);

  delay(8000);

  giroRelativo(90, 20);
  delay(1000);

  Serial2.println("RUN");

  giroRelativo(90, 20);
  delay(1000);

  zerarIMU();
}

void loop() {





  while (true) {
    parar();
    delay(1000);
  }
}