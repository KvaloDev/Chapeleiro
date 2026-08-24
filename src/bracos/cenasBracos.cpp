#include <Arduino.h>
#include "bracosP.h"
#include "cenasBracosD.h"

struct Evento {
  unsigned long tempo;
  void (*acao)();
};

#define EVENTO(tempo, ...) { tempo, []() { __VA_ARGS__ } }

static unsigned long inicioShow = 0;

void iniciarTemporizadorBracos() {
  inicioShow = millis();
}

template <size_t N>
void executarEventos(Evento (&eventos)[N]) {
  for (size_t i = 0; i < N; i++) {
    while (millis() - inicioShow < eventos[i].tempo) {
      moving();
      delay(CICLO_MS);
    }

    eventos[i].acao();
  }

  while (moving()) {
    delay(CICLO_MS);
  }
}

void gesticular1(){
}