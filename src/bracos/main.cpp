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
    delay(250);
    bracoEsquerdo.goTo(50, 55);
    bracoDireito.goTo(50, 55);
    while(moving()){
      int timeOut = 0;
      if(timeOut >= 200){
        break;
      }
      delay(CICLO_MS);
      timeOut += 1;
    }

    bracoEsquerdo.goTo(0, 30);
    bracoDireito.goTo(0, 30);
    while(moving()){
      int timeOut = 0;
      if(timeOut >= 200){
        break;
      }
      delay(CICLO_MS);
      timeOut += 1;
    }
    offBreak();

    //_________________________________________________________________________

    while(msg != "STEP2"){

      if (Serial2.available()) {
        msg = Serial2.readStringUntil('\n');
        msg.trim();
        delay(10);
      }
      if(msg == "STEP2"){
        break;
      }
    } 

    bracoEsquerdo.goTo(50, 55);
    bracoDireito.goTo(50, 55);
    while(moving()){
      int timeOut = 0
      if(timeOut >= 200){
        break;
      }
      delay(CICLO_MS);
      timeOut += 1;
    }

    setServoD(130);
    delay(700);
    setServoD(0);

    bracoEsquerdo.goTo(0, 20);
    bracoDireito.goTo(0, 20);
    while(moving()){
      int timeOut = 0;
      if(timeOut >= 200){
        break;
      }
      delay(CICLO_MS);
      timeOut += 1;
    }
    offBreak();

}

void loop() {}