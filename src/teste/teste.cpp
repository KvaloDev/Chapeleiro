#include <Arduino.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  // Inicia Bluetooth
  SerialBT.begin("BASE_TESTE");
  SerialBT.println("Iniciando...");
}

void loop() {
}