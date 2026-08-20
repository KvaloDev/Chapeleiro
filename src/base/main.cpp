#include "baseP.h"
#include <Arduino.h>
BluetoothSerial SerialBT;
String msg;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("BASE_DEBUG");
  iniciarBroadcast();
  iniciarRecepcaoSpike();
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  initRobot();
  broadcastCanal1(2301);

  SerialBT.println("Iniciando...");

  Serial2.println("STEP1");

run(70, 35);
delay(300);
curva(550, 160, 46);
delay(250);
run(95, 35);
delay(300);
giroRelativo(-180,30);

/* motorE(50, 20);
motorD(50, 20);
while (rodasMovendo()) {
  atualizarMotoresRodas();
  delay(10);
} */

}

void loop() {
  
}