#include "baseP.h"
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <string>

BLEAdvertising* adv;

void ajustarZeroIMU() {
  delay(400);

  atualizarIMU();
  zerarIMU();

  delay(100);
  atualizarIMU();
}

void iniciarBroadcast() {
  BLEDevice::init("");

  adv = BLEDevice::getAdvertising();
  adv->setAdvertisementType(ADV_TYPE_NONCONN_IND);
}

void broadcastCanal1(int16_t radio) {
  std::string dados;

  // LEGO Company ID = 0x0397
  dados.push_back((char)0x97);
  dados.push_back((char)0x03);

  // Canal Pybricks 1
  dados.push_back((char)1);

  // INT de 2 bytes
  dados.push_back((char)((3 << 5) | 2));

  // inteiro little-endian
  dados.push_back((char)(radio & 0xFF));
  dados.push_back((char)((radio >> 8) & 0xFF));

  BLEAdvertisementData pacote;
  pacote.setManufacturerData(dados);

  adv->stop();
  adv->setAdvertisementData(pacote);
  adv->start();
}

// ---------- Estado global ----------
MPU6050 mpu(Wire);

float rotacao = 0;
float rotacaoZero = 0;

volatile long encoder1 = 0;
volatile long encoder2 = 0;

int speed = 50;

// Inicialização

void initRobot() {

  ledcSetup(0, FREQUENCIA, RESOLUCAO);
  ledcSetup(1, FREQUENCIA, RESOLUCAO);
  ledcSetup(2, FREQUENCIA, RESOLUCAO);
  ledcSetup(3, FREQUENCIA, RESOLUCAO);

  ledcAttachPin(RPWM1, 0);
  ledcAttachPin(LPWM1, 1);

  ledcAttachPin(RPWM2, 2);
  ledcAttachPin(LPWM2, 3);

  pinMode(REN1, OUTPUT);
  pinMode(LEN1, OUTPUT);

  pinMode(REN2, OUTPUT);
  pinMode(LEN2, OUTPUT);

  digitalWrite(REN1, HIGH);
  digitalWrite(LEN1, HIGH);

  digitalWrite(REN2, HIGH);
  digitalWrite(LEN2, HIGH);

  pinMode(ENC1_A, INPUT);
  pinMode(ENC2_A, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(ENC1_A),
    contarEncoder1,
    RISING
  );

  attachInterrupt(
    digitalPinToInterrupt(ENC2_A),
    contarEncoder2,
    RISING
  );

  Wire.begin(MPU_SDA, MPU_SCL);

  mpu.begin();
  mpu.calcGyroOffsets(true);

  delay(1000);
}


// ================================================
// Encoders
// ================================================

void contarEncoder1() {
  encoder1++;
}

void contarEncoder2() {
  encoder2++;
}


// ================================================
// Controle direto dos motores
// ================================================

// velocidadeEsquerda e velocidadeDireita:
// -100 até +100
//
// positivo = frente
// negativo = ré

void aplicarMotores(int velocidadeEsquerda, int velocidadeDireita) {

  velocidadeEsquerda = constrain(velocidadeEsquerda, -100, 100);
  velocidadeDireita  = constrain(velocidadeDireita, -100, 100);

  int pwmE = map(abs(velocidadeEsquerda), 0, 100, 0, 255);
  int pwmD = map(abs(velocidadeDireita), 0, 100, 0, 255);


  // MOTOR ESQUERDO
  if (velocidadeEsquerda > 0) {

    ledcWrite(0, 0);
    ledcWrite(1, pwmE);

  } else if (velocidadeEsquerda < 0) {

    ledcWrite(0, pwmE);
    ledcWrite(1, 0);

  } else {

    ledcWrite(0, 0);
    ledcWrite(1, 0);
  }


  // MOTOR DIREITO
  if (velocidadeDireita > 0) {

    ledcWrite(2, pwmD);
    ledcWrite(3, 0);

  } else if (velocidadeDireita < 0) {

    ledcWrite(2, 0);
    ledcWrite(3, pwmD);

  } else {

    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }
}


// ================================================
// Funções básicas
// ================================================

void frente() {
  aplicarMotores(speed, speed);
}


void re() {
  aplicarMotores(-speed, -speed);
}


void girarDireita() {
  aplicarMotores(speed, -speed);
}


void girarEsquerda() {
  aplicarMotores(-speed, speed);
}


void parar() {
  aplicarMotores(0, 0);
}


// ================================================
// IMU
// ================================================

void atualizarIMU() {

  mpu.update();

  rotacao = (mpu.getAngleZ() - rotacaoZero);
}


void zerarIMU() {

  mpu.update();

  rotacaoZero = mpu.getAngleZ();

  rotacao = 0;
}


// ================================================
// RUN
//
// distância positiva = frente
// distância negativa = ré
//
// Ex:
// run(50, 30);
// run(-50, 30);
// ================================================

void run(double cm, int velocidade) {
  offBreak();

  if (cm == 0) {
    brakeRodas();
    return;
  }

  velocidade = constrain(velocidade, VEL_MIN_RUN, 100);

  long inicioE = encoder1;
  long inicioD = encoder2;

  // distância absoluta em mm
  double distanciaMM = fabs(cm) * 10.0;

  // circunferência da roda
  double circRoda = PI * DIAMETRO_RODA;

  // 105 já é pulsos por volta da roda
  long pulsosAlvo =
    (long)(
      (distanciaMM / circRoda)
      * PULSOS_POR_VOLTA
    );

  if (pulsosAlvo < 1) {
    brakeRodas();
    return;
  }

  while (true) {

    long deltaE =
      labs(encoder1 - inicioE);

    long deltaD =
      labs(encoder2 - inicioD);

    // terminou quando as duas chegaram
    if (deltaE >= pulsosAlvo &&
        deltaD >= pulsosAlvo) {
      brakeRodas();
      break;
    }

    // progresso médio
    double progresso =
      ((double)deltaE + (double)deltaD)
      / (2.0 * pulsosAlvo);

    if (progresso > 1.0)
      progresso = 1.0;

    double percorridoCM =
      fabs(cm) * progresso;

    double restanteCM =
      fabs(cm) - percorridoCM;

    // rampa de aceleração
    double fatorAceleracao =
      percorridoCM / RAMPA_RUN_CM;

    if (fatorAceleracao > 1.0)
      fatorAceleracao = 1.0;

    // rampa de desaceleração
    double fatorDesaceleracao =
      restanteCM / RAMPA_RUN_CM;

    if (fatorDesaceleracao > 1.0)
      fatorDesaceleracao = 1.0;

    double fator =
      min(fatorAceleracao, fatorDesaceleracao);

    int velocidadeAtual =
      VEL_MIN_RUN +
      (int)(
        (velocidade - VEL_MIN_RUN)
        * fator
      );

    velocidadeAtual =
      constrain(
        velocidadeAtual,
        VEL_MIN_RUN,
        velocidade
      );


    // ==========================
    // CORREÇÃO PELOS ENCODERS
    // ==========================

    long erroEncoder =
      deltaE - deltaD;

    int correcao =
      (int)(erroEncoder * KP_ALINHAMENTO);
    int velE =
      velocidadeAtual - correcao;

    int velD =
      velocidadeAtual + correcao;

    velE = constrain(
      velE,
      VEL_MIN_RUN,
      velocidade
    );

    velD = constrain(
      velD,
      VEL_MIN_RUN,
      velocidade
    );


    // se uma roda já chegou,
    // não deixa ela continuar passando
    if (deltaE >= pulsosAlvo)
      velE = 0;

    if (deltaD >= pulsosAlvo)
      velD = 0;


    // frente ou ré
    if (cm > 0) {

      aplicarMotores(
        velE,
        velD
      );

    } else {

      aplicarMotores(
        -velE,
        -velD
      );
    }

    delay(10);
  }

  // NÃO chama parar()
  // termina travando as duas rodas
  brakeRodas();
}


// ================================================
// GIRO RELATIVO COM RAMPA
//
// graus positivo = esquerda
// graus negativo = direita
//
// Ex:
// giroRelativo(90, 25);
// giroRelativo(-90, 25);
// ================================================

void giroRelativo(double graus, uint8_t velocidade) {

  offBreak();

  if (graus == 0) {
    parar();
    return;
  }

  velocidade = constrain(velocidade, VEL_MIN_GIRO, 100);

  // Encoder 1 = esquerda
  // Encoder 2 = direita
  long inicioE = encoder1;
  long inicioD = encoder2;

  // Distância que cada roda precisa percorrer no giro
  double distanciaRoda =
    PI * ENTRE_EIXOS * fabs(graus) / 360.0;

  // Circunferência da roda
  double circunferenciaRoda =
    PI * DIAMETRO_RODA;

  // Pulsos por volta da roda após a redução
  double pulsosVoltaRoda =
    PULSOS_POR_VOLTA;

  // Quantos pulsos cada roda precisa percorrer
long pulsosAlvo =
  (long)(
    (distanciaRoda / circunferenciaRoda)
    * PULSOS_POR_VOLTA
    * FATOR_CORRECAO_GIRO
  );

  double total = fabs(graus);

  while (true) {

    long deltaE = labs(encoder1 - inicioE);
    long deltaD = labs(encoder2 - inicioD);

    double pulsosPercorridos =
      (deltaE + deltaD) / 2.0;

    double proporcao =
      pulsosPercorridos / pulsosAlvo;

    if (proporcao > 1.0) {
      proporcao = 1.0;
    }

    double percorrido =
      total * proporcao;

    double restante =
      total - percorrido;


    // ================================================
    // CHEGOU
    // ================================================

    if (deltaE >= pulsosAlvo &&
        deltaD >= pulsosAlvo) {
      break;
      brakeRodas();
    }


    // ================================================
    // RAMPA DE ACELERAÇÃO
    // ================================================

    double fatorAceleracao =
      percorrido / RAMPA_GIRO_GRAUS;

    if (fatorAceleracao > 1.0) {
      fatorAceleracao = 1.0;
    }


    // ================================================
    // RAMPA DE DESACELERAÇÃO
    // ================================================

    double fatorDesaceleracao =
      restante / RAMPA_GIRO_GRAUS;

    if (fatorDesaceleracao > 1.0) {
      fatorDesaceleracao = 1.0;
    }


    double fator =
      min(
        fatorAceleracao,
        fatorDesaceleracao
      );


    int velocidadeAtual =
      VEL_MIN_GIRO +
      (int)(
        (velocidade - VEL_MIN_GIRO)
        * fator
      );

    velocidadeAtual =
      constrain(
        velocidadeAtual,
        VEL_MIN_GIRO,
        velocidade
      );


    int velE = velocidadeAtual;
    int velD = velocidadeAtual;

    // Se uma roda já chegou, para só ela
    if (deltaE >= pulsosAlvo) {
      velE = 0;
    }

    if (deltaD >= pulsosAlvo) {
      velD = 0;
    }


    // ================================================
    // SENTIDO
    // ================================================

    if (graus > 0) {

      aplicarMotores(
        -velE,
         velD
      );

    } else {

      aplicarMotores(
         velE,
        -velD
      );
    }


    Serial.print("E: ");
    Serial.print(deltaE);

    Serial.print(" | D: ");
    Serial.print(deltaD);

    Serial.print(" | Alvo: ");
    Serial.println(pulsosAlvo);


    delay(10);
  }

  parar();
}

void curva(double raioMM, double graus, uint8_t velocidade) {

  offBreak();

  if (graus == 0) {
    brakeRodas();
    return;
  }

  double metadeEixos = ENTRE_EIXOS / 2.0;

  // essa versão faz curva com as duas rodas indo pra frente
  if (raioMM <= metadeEixos) {
    brakeRodas();
    return;
  }

  velocidade = constrain(velocidade, 1, 100);

  long inicioE = encoder1;
  long inicioD = encoder2;

  double angulo = fabs(graus);

  // raio de cada roda
  double raioInterno = raioMM - metadeEixos;
  double raioExterno = raioMM + metadeEixos;

  // distância que cada roda percorre
  double distanciaInterna =
    2.0 * PI * raioInterno * angulo / 360.0;

  double distanciaExterna =
    2.0 * PI * raioExterno * angulo / 360.0;

  double circRoda =
    PI * DIAMETRO_RODA;

  // alvo de encoder
  long pulsosInterno =
    (long)(
      (distanciaInterna / circRoda)
      * PULSOS_POR_VOLTA
    );

  long pulsosExterno =
    (long)(
      (distanciaExterna / circRoda)
      * PULSOS_POR_VOLTA
    );

  if (pulsosInterno < 1)
    pulsosInterno = 1;

  if (pulsosExterno < 1)
    pulsosExterno = 1;


  // --------------------------------
  // PROPORÇÃO REAL DA CURVA
  // --------------------------------

  double proporcao =
    distanciaInterna / distanciaExterna;

  // roda externa recebe a potência pedida
  // roda interna recebe potência proporcional
  double potenciaInterna =
    velocidade * proporcao;

  double potenciaExterna =
    velocidade;


  long alvoE;
  long alvoD;

  double potenciaBaseE;
  double potenciaBaseD;


  if (graus > 0) {

    // CURVA PARA ESQUERDA
    // esquerda = interna
    // direita  = externa

    alvoE = pulsosInterno;
    alvoD = pulsosExterno;

    potenciaBaseE = potenciaInterna;
    potenciaBaseD = potenciaExterna;

  } else {

    // CURVA PARA DIREITA
    // esquerda = externa
    // direita  = interna

    alvoE = pulsosExterno;
    alvoD = pulsosInterno;

    potenciaBaseE = potenciaExterna;
    potenciaBaseD = potenciaInterna;
  }


  while (true) {

    long deltaE =
      labs(encoder1 - inicioE);

    long deltaD =
      labs(encoder2 - inicioD);


    // percentual do trajeto de cada roda
    double progressoE =
      (double)deltaE / (double)alvoE;

    double progressoD =
      (double)deltaD / (double)alvoD;


    // chegou praticamente junto
    if (
      (progressoE >= 1.0 && progressoD >= 0.98) ||
      (progressoD >= 1.0 && progressoE >= 0.98)
    ) {
      break;
    }


    // --------------------------------
    // SINCRONIZAÇÃO DOS ENCODERS
    // --------------------------------

    // positivo = esquerda está adiantada
    // negativo = direita está adiantada
    double erro =
      progressoE - progressoD;

    const double KP_CURVA = 30.0;

    int correcao =
      (int)(erro * KP_CURVA);


    int velE =
      (int)potenciaBaseE - correcao;

    int velD =
      (int)potenciaBaseD + correcao;


    // potência máxima continua sendo
    // a que você passou na função
    velE = constrain(
      velE,
      0,
      velocidade
    );

    velD = constrain(
      velD,
      0,
      velocidade
    );


    // caso uma fique MUITO adiantada,
    // deixa a outra recuperar
    if (progressoE >= 1.0)
      velE = 0;

    if (progressoD >= 1.0)
      velD = 0;


    aplicarMotores(
      velE,
      velD
    );


    SerialBT.print("E: ");
    SerialBT.print(deltaE);
    SerialBT.print("/");
    SerialBT.print(alvoE);

    SerialBT.print(" P=");
    SerialBT.print(velE);

    SerialBT.print(" | D: ");
    SerialBT.print(deltaD);
    SerialBT.print("/");
    SerialBT.print(alvoD);

    SerialBT.print(" P=");
    SerialBT.println(velD);


    delay(10);
  }

  brakeRodas();
}

void brakeRodas() {

  digitalWrite(REN1, HIGH);
  digitalWrite(LEN1, HIGH);
  digitalWrite(REN2, HIGH);
  digitalWrite(LEN2, HIGH);

  ledcWrite(0, 255);
  ledcWrite(1, 255);

  ledcWrite(2, 255);
  ledcWrite(3, 255);
}

void offBreak() {

  ledcWrite(0, 0);
  ledcWrite(1, 0);

  ledcWrite(2, 0);
  ledcWrite(3, 0);

  digitalWrite(REN1, HIGH);
  digitalWrite(LEN1, HIGH);

  digitalWrite(REN2, HIGH);
  digitalWrite(LEN2, HIGH);
}

// ================================================
// CONTROLE INDIVIDUAL NÃO BLOQUEANTE DAS RODAS
// ================================================

static bool rodaEAtiva = false;
static bool rodaDAtiva = false;

static long inicioMotorE = 0;
static long inicioMotorD = 0;

static long alvoMotorE = 0;
static long alvoMotorD = 0;

static int comandoMotorE = 0;
static int comandoMotorD = 0;


// Converte centímetros em pulsos
static long cmParaPulsos(double cm) {

  double distanciaMM = fabs(cm) * 10.0;

  double circRoda =
    PI * DIAMETRO_RODA;

  return (long)(
    (distanciaMM / circRoda)
    * PULSOS_POR_VOLTA
  );
}


// ================================================
// MOTOR ESQUERDO
// ================================================

void motorE(double cm, int potencia) {

  potencia = constrain(abs(potencia), 0, 100);

  if (cm == 0 || potencia == 0) {
    rodaEAtiva = false;
    comandoMotorE = 0;
    return;
  }

  inicioMotorE = encoder1;

  alvoMotorE =
    cmParaPulsos(cm);

  if (cm > 0) {
    comandoMotorE = potencia;
  } else {
    comandoMotorE = -potencia;
  }

  rodaEAtiva = true;
}


// ================================================
// MOTOR DIREITO
// ================================================

void motorD(double cm, int potencia) {

  potencia = constrain(abs(potencia), 0, 100);

  if (cm == 0 || potencia == 0) {
    rodaDAtiva = false;
    comandoMotorD = 0;
    return;
  }

  inicioMotorD = encoder2;

  alvoMotorD =
    cmParaPulsos(cm);

  if (cm > 0) {
    comandoMotorD = potencia;
  } else {
    comandoMotorD = -potencia;
  }

  rodaDAtiva = true;
}


// ================================================
// UPDATE DAS DUAS RODAS
// ================================================

void atualizarMotoresRodas() {

  int velE = 0;
  int velD = 0;


  // ---------- ESQUERDA ----------

  if (rodaEAtiva) {

    long percorridoE =
      labs(encoder1 - inicioMotorE);

    if (percorridoE >= alvoMotorE) {

      rodaEAtiva = false;
      comandoMotorE = 0;

    } else {

      velE = comandoMotorE;
    }
  }


  // ---------- DIREITA ----------

  if (rodaDAtiva) {

    long percorridoD =
      labs(encoder2 - inicioMotorD);

    if (percorridoD >= alvoMotorD) {

      rodaDAtiva = false;
      comandoMotorD = 0;

    } else {

      velD = comandoMotorD;
    }
  }


  // mantém BTS habilitados
  digitalWrite(REN1, HIGH);
  digitalWrite(LEN1, HIGH);
  digitalWrite(REN2, HIGH);
  digitalWrite(LEN2, HIGH);


  // controla as duas de UMA vez
  aplicarMotores(
    velE,
    velD
  );


  // quando as duas terminarem, trava
  if (!rodaEAtiva && !rodaDAtiva) {
    brakeRodas();
  }
}


// ================================================
// VERIFICA SE AINDA TEM RODA MOVENDO
// ================================================

bool rodasMovendo() {

  return rodaEAtiva || rodaDAtiva;
}