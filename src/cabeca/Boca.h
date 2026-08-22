#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>

class Boca {
  private:
    Servo servoBoca;

  public:
    void inicializar();
    
    // Função base: recebe um ângulo (0-180) e manda o pulso PWM para o REV Servo
    void moverMotor(int angulo); 
    
    void abrirBoca();
    void fecharBoca();

    // Função orgânica: recebe 0 a 100%
    void falarPorcento(int porcentagem);
};