#pragma once

#include <ESP32Servo.h>

class Olhos{

    private:

    Servo servoX;
    Servo servoY;

    public:

    void inicializar();

    void mover(int anguloX, int anguloY);
};