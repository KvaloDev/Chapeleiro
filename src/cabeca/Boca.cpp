#include "Boca.h"
#include "Config.h"

void Boca::inicializar() {
    servoBoca.attach(pinoBoca);
    fecharBoca();
}

// A função base que estava faltando! Ela fala com o hardware.
void Boca::moverMotor(int angulo) {
    // Segurança: trava o ângulo físico entre 0 e 180 para não quebrar a peça
    angulo = constrain(angulo, 0, 180);

    // Mágica: Converte o ângulo (0-180) para microsegundos do REV Servo (500-2500)
    int microsegundos = map(angulo, 0, 180, pulsoMinBoca, pulsoMaxBoca);
    
    servoBoca.writeMicroseconds(microsegundos);
}

void Boca::abrirBoca() {
    moverMotor(anguloBocaAberta);
}

void Boca::fecharBoca() {
    moverMotor(anguloBocaFechada);
}

void Boca::falarPorcento(int porcentagem) {
    // Segurança: trava o valor recebido da tabela entre 0 e 100%
    porcentagem = constrain(porcentagem, 0, 100);
    
    // Converte os 0-100% do áudio para os ângulos físicos que você ajustou no Config.h
    int anguloAlvo = map(porcentagem, 0, 100, anguloBocaFechada, anguloBocaAberta);
    
    // Chama a nossa função base para mexer o motor de verdade
    moverMotor(anguloAlvo);
}