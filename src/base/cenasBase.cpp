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
      giroRelativo(195, 30); ),
  };
  executarEventos(eventos);
}

void cena2() {
  Evento eventos[] = {
    EVENTO(42100, run(80, 70); ),
    EVENTO(58088, run(-40, 60);
    delay(500);
    run(40, 25); ),
    EVENTO(65970, giroRelativo(-110, 35);
    delay(350); ),
    EVENTO(67207, giroRelativo(110, 35); )
  };
  executarEventos(eventos);
}