#include "baseP.h"

BluetoothSerial SerialBT;
String msg;

void setup() {

  //delay(10000);

  Serial.begin(115200);
  SerialBT.begin("BASE_DEBUG");

  initRobot();

  SerialBT.println("Corre neguin");
  delay(4000);
 

  run(25, 35);
 // SerialBT.println("RUN(20,40)");
   giroRelativo(90, 40);
   delay(1000);
  SerialBT.println("GIRO RELATIVO(90,40)");
 /*  while(true){
    parar();
    delay(999999);
  } */

   Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
   Serial2.println("Serial2 started");

  delay(1000);

 // giroRelativo(90, 40);
 // SerialBT.println("GIRO RELATIVO(90,40)");
  //delay(1000);

  Serial2.println("STEP1");


      /*  while(msg != "STEP2"){
      if (Serial2.available()) {
        msg = Serial2.readStringUntil('\n');
      }
    }   */

    SerialBT.println("STEP2 recebido");

  //giroRelativo(-90, 40);
  SerialBT.println("GIRO RELATIVO(-90,40)");
  delay(1000); 

  zerarIMU(); 
}



void loop() {
 
  atualizarIMU();
  SerialBT.println(rotacao);
  
}