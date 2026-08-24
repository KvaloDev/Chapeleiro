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
    EVENTO(14075, run(70, 75);
      curva(550, 175, 60);
      run(85, 70);
      setPosition(90, 30); )
  };
  executarEventos(eventos);
}

void cena2() {
  Evento eventos[] = {
    EVENTO(42100, run(80, 65); ),
    EVENTO(58000, run(-55, 30); ),
    EVENTO(65100, setPosition(0, 35); 
    delay(300);
    run(-80, 35);
    delay(250);
    setPosition(90, 35); 
    run(-40, 35);)
  };
  executarEventos(eventos);
}

void cena3(){
    Evento eventos[] = {
      EVENTO(103419, run(25, 25); )
    };
  executarEventos(eventos);
}

void horaDoCha(){
    Evento eventos[] = {
        EVENTO(114800, broadcastCanal(120, 2565); )
    };
     executarEventos(eventos);
}