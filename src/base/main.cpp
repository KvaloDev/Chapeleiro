#include "baseP.h"
#include <Arduino.h>
BluetoothSerial SerialBT;
String msg;

void setup() {
  delay(2000);
  Serial.begin(115200);
  SerialBT.begin("BASE_DEBUG");
  iniciarBroadcast();
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  initRobot();
  ajustarZeroIMU();
  broadcastCanal1(2301);
SerialBT.println("Iniciando...");


  delay(2000);
  run(50, 30);
  delay(1000);
  giroRelativo(180, 30);
  delay(1000);
  run(50, 30);
  delay(1000);
 
   Serial2.println("STEP1");
   delay(1000);
  
while(true){
    delay(1000);
  }
   
   giroRelativo(180, 40);
   Serial2.println("STEP2");
   delay(500);

   run(90, 40);
   delay(1000);

  zerarIMU(); 
}

void loop() {

  atualizarIMU();
  SerialBT.println(rotacao);
  
}