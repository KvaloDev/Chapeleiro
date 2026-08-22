#include "Pescoco.h"
#include "Config.h"


void Pescoco::incializar(){
    servoPan.attach(pinoPescocoPan);
    servoTilt.attach(pinoPescocoTilt);

    centralizarPan();
    olharFrente();
}

void Pescoco::moverPan(int angulo){
    angulo = constrain(angulo, 0, 180);

    int microsegundos = map(angulo, 0, 180, pulsoMinPescoco, pulsoMaxPescoco);

    servoPan.writeMicroseconds(microsegundos);
}

void Pescoco::olharEsquerda(){
    moverPan(anguloPanEsquerda);
}

void Pescoco::olharDireita(){
    moverPan(anguloPanDireita);
}

void Pescoco::centralizarPan(){
    moverPan(anguloPanCentro);
}




void Pescoco::moverTilt(int angulo){
    angulo = constrain(angulo, 0, 180);
    int microsegundos = map(angulo, 0, 180, pulsoMinPescoco, pulsoMaxPescoco);
    servoTilt.writeMicroseconds(microsegundos);
}

void Pescoco::olharFrente(){
    moverTilt(anguloTiltFrente);
}

void Pescoco::olharBaixo(){
    moverTilt(anguloTiltBaixo);
}