#include <Arduino.h>
#include "bracosP.h"

void setup() {
  Serial.begin(115200);
  iniciarBracos();
  String msg;

 Serial2.begin(9600, SERIAL_8N1, 16, 17);


    while(msg != "RUN"){
      if (Serial2.available()) {
        msg = Serial2.readStringUntil('\n');
      }
    } 
  Serial.println("SET MOTOR");
  setMotorE(90, 80);
  Serial.println("ESPERA CONCLUIDA!");
}


void loop() {
  processarSerial();

 /*  if (!modoConsole) {

    setMotorE(90, 40);
    setMotorD(90, 40);   // vai para 90° com 80% de potência

    setMotorE(0, 40);
    setMotorD(0, 40);

    delay(1000); 
  }*/
}