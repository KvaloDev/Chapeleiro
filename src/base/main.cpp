#include "baseP.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("BASE_DEBUG");

  initRobot();

  SerialBT.println("START SETUP");




  /* Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  delay(5000);

  giroRelativo(90, 40);
  delay(1000);

  Serial2.println("RUN");

  giroRelativo(-90, 40);
  delay(1000); */

  zerarIMU();
}



void loop() {
 
  atualizarIMU();
  SerialBT.println(rotacao);
  
}