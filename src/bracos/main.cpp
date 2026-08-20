#include <Arduino.h>
#include "bracosP.h"

void setup() {
Serial.begin(115200);
Serial2.begin(9600, SERIAL_8N1, 16, 17);
iniciarBracos();
iniciarServos();
String msg; 
int x;

while(msg != "STEP1"){

      if (Serial2.available()) {
        msg = Serial2.readStringUntil('\n');
        msg.trim();
        delay(10);
      }
      if(msg == "STEP1"){
        break;
      }
    }  

    setServoD(65);
    bracoEsquerdo.goTo(90, 45);
    bracoDireito.goTo(90, 45);
    while(moving()){
      delay(CICLO_MS);
    }

  

    bracoEsquerdo.goTo(10, 45);
    bracoDireito.goTo(10, 45);
    setServoD(0);
    while(moving()){
      delay(CICLO_MS);
    }
    
    while(true){
    delay(1000);
  }

while (msg != "STEP2") {
  bracoEsquerdo.goTo(50, 50);
  bracoDireito.goTo(50, 50);
  while (moving() && msg != "STEP2") {
    if (Serial2.available()) {
      msg = Serial2.readStringUntil('\n');
    }
  }
  if (msg == "STEP2") break;
  bracoEsquerdo.goTo(0, 50);
  bracoDireito.goTo(0, 50);
  while (moving() && msg != "STEP2") {
    if (Serial2.available()) {
      msg = Serial2.readStringUntil('\n');
    }
  }
}
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