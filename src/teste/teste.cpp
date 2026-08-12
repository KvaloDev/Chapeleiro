#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include "BluetoothSerial.h"

#define MPU_SDA 21
#define MPU_SCL 22

MPU6050 mpu(Wire);
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  // Inicia I2C
  Wire.begin(MPU_SDA, MPU_SCL);

  // Inicia MPU
  mpu.begin();

  // Inicia Bluetooth
  SerialBT.begin("MPU_TESTE");

  // Espera estabilizar
  delay(3000);

  SerialBT.println("NAO MOVA O MPU");
  Serial.println("NAO MOVA O MPU");

  // Calibra giroscopio
  mpu.calcGyroOffsets(true);

  delay(1000);

  SerialBT.println("CALIBRADO");
  Serial.println("CALIBRADO");
}

void loop() {
  mpu.update();

  SerialBT.print("GyroZ: ");
  SerialBT.print(mpu.getGyroZ(), 4);

  SerialBT.print(" | AngleZ: ");
  SerialBT.println(mpu.getAngleZ(), 2);

  // Mantém também no USB caso esteja conectado
  Serial.print("GyroZ: ");
  Serial.print(mpu.getGyroZ(), 4);

  Serial.print(" | AngleZ: ");
  Serial.println(mpu.getAngleZ(), 2);

  delay(100);
}