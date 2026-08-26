#include <Arduino.h>
#include "bracosP.h"
#include "cenasBracosD.h"

void setup() {

Serial.begin(115200);
Serial2.begin(9600, SERIAL_8N1, 16, 17);
iniciarBracos();
iniciarServos();
int timeOut = 0;

ah();
gesticular1();
gesticular2();
gesticular3();
horaDoCha();
gesticular4();
gesticular5();

}

void loop() {
  void endBracos();

 while(true){
  delay(250);
 }
}