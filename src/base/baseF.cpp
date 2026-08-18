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

  if (cm == 0) {
    parar();
    return;
  }

  velocidade = constrain(velocidade, VEL_MIN_RUN, 100);


  // --------------------------------
  // Define direção
  // --------------------------------

  int direcao = 1;

  if (cm < 0) {
    direcao = -1;
  }


  double distanciaAbs = fabs(cm);


  // --------------------------------
  // Zera encoders
  // --------------------------------

  encoder1 = 0;
  encoder2 = 0;


  // --------------------------------
  // Calcula alvo
  // --------------------------------

  double distancia_mm = distanciaAbs * 10.0;

  double circunferencia =
    PI * DIAMETRO_RODA;

  double voltas =
    distancia_mm / circunferencia;

  long alvo =
    voltas * PULSOS_POR_VOLTA;


  if (alvo <= 0) {
    parar();
    return;
  }


  // --------------------------------
  // Quantos pulsos correspondem
  // à região da rampa
  // --------------------------------

  double rampa_mm =
    RAMPA_RUN_CM * 10.0;

  double voltasRampa =
    rampa_mm / circunferencia;

  long pulsosRampa =
    voltasRampa * PULSOS_POR_VOLTA;


  // Nunca deixa a rampa maior
  // que metade do movimento
  if (pulsosRampa > alvo / 2) {
    pulsosRampa = alvo / 2;
  }

  if (pulsosRampa < 1) {
    pulsosRampa = 1;
  }


  // --------------------------------
  // Referência de direção
  // --------------------------------

  zerarIMU();


  // ================================================
  // LOOP DO MOVIMENTO
  // ================================================

  while (true) {

    long e1 = encoder1;
    long e2 = encoder2;

    long atual =
      (labs(e1) + labs(e2)) / 2;


    if (atual >= alvo) {
      break;
    }


    long restante =
      alvo - atual;


    // ================================================
    // RAMPA DE ACELERAÇÃO
    // ================================================

    double fatorAceleracao = 1.0;

    if (atual < pulsosRampa) {

      fatorAceleracao =
        (double)atual /
        (double)pulsosRampa;
    }


    // ================================================
    // RAMPA DE DESACELERAÇÃO
    // ================================================

    double fatorDesaceleracao = 1.0;

    if (restante < pulsosRampa) {

      fatorDesaceleracao =
        (double)restante /
        (double)pulsosRampa;
    }


    // Usa o menor fator
    double fator =
      min(
        fatorAceleracao,
        fatorDesaceleracao
      );


    // ================================================
    // Velocidade atual
    // ================================================

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


    // ================================================
    // CORREÇÃO COM GIROSCÓPIO
    // ================================================

    atualizarIMU();


    // rotacao > 0:
    // robô desviou para esquerda
    // então força giro para direita

    float correcao =
      rotacao * KP_ALINHAMENTO;


    // Evita uma correção absurda
    correcao =
      constrain(
        correcao,
        -15.0f,
        15.0f
      );


    // ================================================
    // Velocidade de cada roda
    // ================================================

    int velE =
      direcao * velocidadeAtual;

    int velD =
      direcao * velocidadeAtual;


    velE += correcao;
    velD -= correcao;


    velE =
      constrain(
        velE,
        -100,
        100
      );

    velD =
      constrain(
        velD,
        -100,
        100
      );


    aplicarMotores(
      velE,
      velD
    );


    delay(10);
  }


  parar();
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