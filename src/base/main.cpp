#include "baseP.h"
BluetoothSerial SerialBT;
String msg;

void setup() {

  Serial.begin(115200);
  SerialBT.begin("BASE_DEBUG");
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  initRobot();
  while(true){
    delay(1000);
  }

   Serial2.println("STEP1");
   run(90, 40);
   delay(1000);
  
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