#include <Arduino.h>
#include "bracosP.h"

void setup() {
  Serial.begin(115200);
  iniciarBracos();
  delay(1000);
  String msg;

 Serial2.begin(9600, SERIAL_8N1, 16, 17);

 Serial.println("Esperando STEP1...");

     while(msg != "STEP1"){
      if (Serial2.available()) {
        msg = Serial2.readStringUntil('\n');
        msg.trim();
        delay(10);
      }
      if(msg == "STEP1"){
       Serial.println("STEP1 recebido");
        delay(1000);
        break;
      }
    }  
  Serial2.println("STEP2");
  setMotorE(90, 40);
  setMotorE(0, 40);
  setMotorE(90, 40);
  

       while(msg != "STEP3"){
      if (Serial2.available()) {
        msg = Serial2.readStringUntil('\n');
      }
    }  

    setMotorD(90, 40);
    setMotorD(0, 90);
}


void loop() {
 


 /*  if (!modoConsole) {

    setMotorE(90, 40);
    setMotorD(90, 40);   // vai para 90° com 80% de potência

    setMotorE(0, 40);
    setMotorD(0, 40);

    delay(1000); 
  }*/
}