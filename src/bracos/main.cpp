#include <Arduino.h>
#include "bracosP.h"
#include "cenasBracosD.h"

void setup() {
Serial.begin(115200);
Serial2.begin(9600, SERIAL_8N1, 16, 17);
iniciarBracos();
iniciarServos();
String msg; 
int x;
int timeOut = 0;

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
    bracoEsquerdo.goTo(50, 50);
    bracoDireito.goTo(50, 50);
    timeOut = 0;
    while(moving()){
      if(timeOut >= 250){
        break;
      }
      delay(CICLO_MS);
      timeOut += 1;
    }

    bracoEsquerdo.goTo(0, 30);
    bracoDireito.goTo(0, 30);
    timeOut = 0;
    while(moving()){
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

    bracoEsquerdo.goTo(90, 65);
    bracoDireito.goTo(0, 65);
    timeOut = 0;
    while(moving()){
      if(timeOut >= 200){
        break;
      }
      delay(CICLO_MS);
      timeOut += 1;
    }

    delay(800);
    bracoEsquerdo.goTo(0, 65);
    timeOut = 0;
    while(moving()){
      if(timeOut >= 200){
        break;
      }
      delay(CICLO_MS);
      timeOut += 1;
    }


}

void loop() {}