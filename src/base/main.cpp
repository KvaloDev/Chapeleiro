#include "baseP.h"
#include <Arduino.h>
#include "cenasBaseD.h"

BluetoothSerial SerialBT;

void setup() {
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  SerialBT.begin("espBase");
  iniciarTemporizador();
  //Serial.begin(115200);
  iniciarBroadcast();
  iniciarRecepcaoSpike();
  initRobot();
  broadcastCanal1(2301);

  cena1();
  cena2();
  horaDoCha();
}

void loop() {
  while(true){
    delay(200);
  }
}