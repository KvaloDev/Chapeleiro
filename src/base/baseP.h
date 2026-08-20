#pragma once

#include <Arduino.h>
#include <math.h>
#include "BluetoothSerial.h"

extern BluetoothSerial SerialBT;

void iniciarBroadcast();
void broadcastCanal1(int16_t radio);
void curva(double raioMM, double graus, uint8_t velocidade);
void brakeRodas();
void motorE(double cm, int potencia);
void motorD(double cm, int potencia);

void atualizarMotoresRodas();
bool rodasMovendo();

// UART 
#define UART_RX 16
#define UART_TX 17

//  BTS1 Esquerda
#define RPWM1 25
#define LPWM1 26
#define REN1  27
#define LEN1  14

//  BTS 2 (Direita)
#define RPWM2 32
#define LPWM2 33
#define REN2  18
#define LEN2  19

// Encoder 1 (Esquerda)
#define ENC1_A 34
#define ENC1_B 35

// Encoder 2 (Direita)
#define ENC2_A 23
#define ENC2_B 13

// PWM
#define FREQUENCIA 1000
#define RESOLUCAO   8

// Dados físicos 
#define DIAMETRO_RODA      90.0
#define ENTRE_EIXOS        316.0
#define REDUCAO            15
#define PULSOS_POR_VOLTA   105

// Tamanho da rampa
#define RAMPA_RUN_CM        50.0
#define RAMPA_GIRO_GRAUS    45.0

#define TIMEOUT_GIRO_MS 5000
#define TOLERANCIA_GIRO 0.9
#define FATOR_CORRECAO_GIRO 1.1

// Velocidade mínima
#define VEL_MIN_RUN         25
#define VEL_MIN_GIRO        13

// Correção
#define KP_ALINHAMENTO      1.1

// Estado 
extern volatile long encoder1;
extern volatile long encoder2;

extern int speed;

// Inicialização 
void initRobot();

// Encoders 
void contarEncoder1();
void contarEncoder2();

// Motores
void frente();
void re();
void girarDireita();
void girarEsquerda();
void parar();

// Movimento
void run(double cm, int velocidade);

void offBreak();

void giroRelativo(double graus, uint8_t velocidade);

void iniciarRecepcaoSpike();

double getAnguloSpike();

double erroAngular(
    double alvo,
    double atual
);

void setPosition(
    double graus,
    uint8_t velocidade
);