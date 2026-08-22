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
    EVENTO(14075,
      Serial2.println ("STEP1");
      run(70, 75);
      curva(550, 175, 60);
      run(85, 70);
      giroRelativo(195, 30); 
      Serial2.println("STEP2"); )
  };
  executarEventos(eventos);
}

void cena2() {
  Evento eventos[] = {
    EVENTO(42100, run(80, 65); ),
    EVENTO(58088, run(-45, 45);
    delay(500); ),
    EVENTO(65970, Serial2.println("STEP2");
    giroRelativo(-42, 35);
    run(-90, 40); ),
    EVENTO(67300, Serial2.println("STEP3");
    setPosition(93, 35);)
  };
  executarEventos(eventos);
}

void cena3(){
    Evento eventos[] = {
      EVENTO(103419, giroRelativo(15, 35);
      run(25, 25); )
    };
  executarEventos(eventos);
}

void horaDoCha(){
    Evento eventos[] = {
        EVENTO(114800, broadcastCanal(120, 2565); )
    };
     executarEventos(eventos);
}