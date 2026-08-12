#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include <math.h>
#include "BluetoothSerial.h"

extern BluetoothSerial SerialBT;

// ---------- UART ----------
#define UART_RX 16
#define UART_TX 17

// ---------- MPU6050 ----------
#define MPU_SDA 21
#define MPU_SCL 22

// ---------- BTS 1 (Esquerda) ----------
#define RPWM1 25
#define LPWM1 26
#define REN1  27
#define LEN1  14

// ---------- BTS 2 (Direita) ----------
#define RPWM2 32
#define LPWM2 33
#define REN2  18
#define LEN2  19

// ---------- Encoder 1 (Esquerda) ----------
#define ENC1_A 34
#define ENC1_B 35

// ---------- Encoder 2 (Direita) ----------
#define ENC2_A 23
#define ENC2_B 13

// ---------- PWM ----------
#define FREQUENCIA 1000
#define RESOLUCAO   8

// ---------- Dados físicos ----------
#define DIAMETRO_RODA      90.0
#define ENTRE_EIXOS        316.0
#define REDUCAO            15
#define PULSOS_POR_VOLTA   105

// ================================================
// AJUSTES DE MOVIMENTO
// ================================================

// Quanto MAIOR, mais longa/suave a aceleração e desaceleração
#define RAMPA_RUN_CM        20.0
#define RAMPA_GIRO_GRAUS    25.0

// Velocidade mínima durante a rampa (%)
#define VEL_MIN_RUN         10
#define VEL_MIN_GIRO        12

// Correção do MPU enquanto anda reto
#define KP_ALINHAMENTO      1.2

// Tolerância final do giro
#define TOLERANCIA_GIRO     1.5

#define TIMEOUT_GIRO_MS 4000

// ---------- Estado global ----------
extern MPU6050 mpu;

extern float rotacao;
extern float rotacaoZero;

extern volatile long encoder1;
extern volatile long encoder2;

extern int speed;

// ---------- Inicialização ----------
void initRobot();

// ---------- Encoders ----------
void contarEncoder1();
void contarEncoder2();

// ---------- Motores ----------
void frente();
void re();
void girarDireita();
void girarEsquerda();
void parar();

// ---------- Movimento ----------
void run(double cm, int velocidade);

// ---------- IMU ----------
void atualizarIMU();
void zerarIMU();

void giroRelativo(double graus, uint8_t velocidade);