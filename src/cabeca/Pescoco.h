#pragma once

#include <ESP32Servo.h>

class Pescoco{
    private:

    Servo servoPan;
    Servo servoTilt;

    public:

    void incializar();

    void moverPan(int angulo);
    void moverTilt(int angulo);

    void olharEsquerda();
    void olharDireita();
    void centralizarPan();

    void olharFrente();
    void olharBaixo();
};