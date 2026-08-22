#include "Olhos.h"
#include "Config.h"

void Olhos::inicializar(){
    
    servoX.attach(pinoOlhoX);
    servoY.attach(pinoOlhoY);

    servoX.write(90);
    servoY.write(70);

}

void Olhos::mover(int anguloX, int anguloY){

    servoX.write(anguloX);
    servoY.write(anguloY);
}