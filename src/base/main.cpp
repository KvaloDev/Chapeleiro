#include "baseP.h"
#include <Arduino.h>
#include "cenasBaseD.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  SerialBT.begin("BASE_DEBUG");

  iniciarBroadcast();
  iniciarRecepcaoSpike();

  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  initRobot();

  broadcastCanal1(2301);

  /*
  if (!esperarSpike(3000)) {
    while (true) {
      delay(1000);
    }
  }
  */

  iniciarTemporizador();

  cena1();
  cena2();
  cena3();
  horaDoCha();

  setupOTA("chapeleiro-base");

  xTaskCreatePinnedToCore(otaTask, "OTA", 4096, NULL, 1, NULL, 0);
}

void loop() {
}