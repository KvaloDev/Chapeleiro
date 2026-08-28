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

void teste(){
  Evento eventos[] = {
    EVENTO(1500, setServos(70, 70);
     goToBracos(50, 50, 50); )
  };
  executarEventos(eventos);
}

void ah(){
  Evento eventos[] = {
    EVENTO(14314, setServoE(30); setServoD(30);
     goToBracos(65, 65, 50); ),
    EVENTO(15520, setServoE(0); setServoD(0); 
     goToBracos(0, 0, 35); ),
    EVENTO(16950, offBreak(); )
  };
  executarEventos(eventos);
}

void gesticular1(){
  Evento eventos[] = {
    EVENTO(26091, goToBracos(35, 35, 60);
     setServos(35, 35); ),
    EVENTO(27690, setServos(50, 7);
     goToBracos(0, 90, 60); ),
    EVENTO(28690, setServos(45, 45);
     goToBracos(35, 35, 60);),
    EVENTO(33584, goToBracos(60, 60, 50);),
    EVENTO(34284, goToBracos(70, 70 ,50);),
    EVENTO(35662, goToBracos(50, 50, 50);
     setServos(80, 80); ),
    EVENTO(38500, setServos(38, 38);
     goToBracos(110, 110, 65); ),
    EVENTO(39900, setServos(0, 0); 
     goToBracos(0, 0, 50); ),
    EVENTO(41000, offBreak(); )
  };
  executarEventos(eventos);
}

void gesticular2(){
  Evento eventos[] = {
    EVENTO(43338, goToBracos(55, 55, 55);
     setServos(50, 50); ),
    EVENTO(45105, setServos(70, 70);
     goToBracos(100, 100, 65); ),
    EVENTO(46051, goToBracos(0, 0, 30); 
     setServos(0, 0); ),
    EVENTO(47036, goToBracos(80, 80, 65);
     setServos(35, 35); ),
    EVENTO(49000, goToBracos(50, 50, 45); ),
    EVENTO(52412, goToBracos(75, 75, 65);
     setServos(45, 45); ),
    EVENTO(55211, goToBracos(50, 50, 45); ),
    EVENTO(55847, goToBracos(20, 20, 40); ),
    EVENTO(57458, goToBracos(0, 0, 40); 
     setServos(0, 0); ),
    EVENTO(58800, offBreak(); )
  };
  executarEventos(eventos);
}

void gesticular3(){
  Evento eventos[] = {
    EVENTO(59541, goToBracos(65, 65, 65);
     setServos(45, 45); ),
    EVENTO(60978, goToBracos(40, 40, 50); ),
    EVENTO(65770, goToBracos(0, 95, 65);
     setServos(0, 25); ),
    EVENTO(68000, goToBracos(0, 0, 45); ),
    EVENTO(69300, offBreak(); )
  };
  executarEventos(eventos);
}

void gesticular4(){
  Evento eventos[] = {
  EVENTO(112539, goToBracos(55, 55, 55);
   setServos(40, 40); ),
  EVENTO(114839, goToBracos(0, 0, 50);
   setServos(0, 0); ),
  EVENTO(115600, offBreak(); )
  };
  executarEventos(eventos);
}

void gesticular5(){
  Evento eventos[] = {
    EVENTO(138706, goToBracos(65, 65, 55);
     setServos(50, 50); ),
    EVENTO(140911, goToBracos(0, 0, 45); 
     setServos(0, 0); ),
    EVENTO(141900, offBreak(); ),
    EVENTO(184884, goToBracos(65, 65, 60);
     setServos(65, 65); ),
    EVENTO(186920, goToBracos(0, 0, 50);
     setServos(0, 0); ),
    EVENTO(188200, offBreak(); )
  };
  executarEventos(eventos);
}

void horaDoCha(){
  Evento eventos[] = {
    EVENTO(112050, goToBracos(80, 0, 60);
     setServoE(80); ),
    EVENTO(114700, setServoE(0);
     goToBracos(0, 0, 60); ),
    EVENTO(115700, offBreak(); )
    };
    executarEventos(eventos);
}