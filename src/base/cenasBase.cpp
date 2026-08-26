#include "baseP.h"
#include "cenasBaseD.h"

struct Evento {
  unsigned long tempo;
  void (*acao)();
};

#define EVENTO(tempo, ...) { tempo, []() { __VA_ARGS__ } }

static unsigned long inicioShow = 0;

void iniciarTemporizador() {
  inicioShow = millis();
}

template <size_t N>
void executarEventos(Evento (&eventos)[N]) {
  for (size_t i = 0; i < N; i++) {
    while (millis() - inicioShow < eventos[i].tempo) {
      delay(1);
    }

    eventos[i].acao();
  }
}

void cena1() {
  Evento eventos[] = {
    EVENTO(14075, run(60, 75);
      curva(550, 175, 65);
      run(85, 75);
      delay(250);
      setPosition(90, 35); )
  };
  executarEventos(eventos);
}

void cena2() {
  Evento eventos[] = {
    EVENTO(42100, run(80, 65); ),
    EVENTO(58000, run(-65, 30); ),
    EVENTO(64900, setPosition(0, 35); 
    delay(400);
    run(-80, 35);
    delay(450);
    setPosition(90, 35);
    delay(450);
    run(-40, 35); )
  };
  executarEventos(eventos);
}

void cena3(){
     Evento eventos[] = {
      EVENTO(80097, run(15, 35); ) 
    };
  executarEventos(eventos); 
}

void horaDoCha(){
    Evento eventos[] = {
      EVENTO(114800, broadcastCanal(120, 2565);
       setPosition(135, 40);
       delay(350);
       run(60, 70);
       delay(350);
       setPosition(88, 40);
       delay(350);
       run(80, 70);
       delay(350);
       setPosition(0, 40);
       delay(350);
       run(80, 70);
       delay(350);
       setPosition(-135, 40); )

    };
     executarEventos(eventos);
}