#include "baseP.h"
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <string>

#define CANAL_ANGULO_SPIKE 8

BLEAdvertising* adv;
BLEScan* scanSpike = nullptr;

volatile int32_t anguloSpikeX10 = 0;
volatile unsigned long ultimoSpike = 0;
volatile bool spikeRecebido = false;

volatile long encoder1 = 0;
volatile long encoder2 = 0;

int speed = 50;

bool decodificarIntPybricks(
  const std::string& dados,
  uint8_t canalEsperado,
  int32_t& valor
) {
  if (dados.size() < 5) return false;

  const uint8_t* d = (const uint8_t*)dados.data();
  size_t tamanho = dados.size();

  if (d[0] != 0x97 || d[1] != 0x03) return false;
  if (d[2] != canalEsperado) return false;

  size_t i = 3;

  if (i < tamanho && d[i] == 0x00) i++;
  if (i >= tamanho) return false;

  uint8_t header = d[i++];
  uint8_t tipo = header >> 5;
  uint8_t bytes = header & 0x1F;

  if (tipo != 3) return false;
  if (i + bytes > tamanho) return false;

  if (bytes == 1) {
    valor = (int8_t)d[i];
    return true;
  }

  if (bytes == 2) {
    uint16_t bruto =
      ((uint16_t)d[i]) |
      ((uint16_t)d[i + 1] << 8);

    valor = (int16_t)bruto;
    return true;
  }

  if (bytes == 4) {
    uint32_t bruto =
      ((uint32_t)d[i]) |
      ((uint32_t)d[i + 1] << 8) |
      ((uint32_t)d[i + 2] << 16) |
      ((uint32_t)d[i + 3] << 24);

    valor = (int32_t)bruto;
    return true;
  }

  return false;
}

class SpikeCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice device) override {
    if (!device.haveManufacturerData()) return;

    std::string dados = device.getManufacturerData();
    int32_t valor;

    if (decodificarIntPybricks(dados, CANAL_ANGULO_SPIKE, valor)) {
      anguloSpikeX10 = valor;
      ultimoSpike = millis();
      spikeRecebido = true;
    }
  }
};

void iniciarBroadcast() {
  BLEDevice::init("");

  adv = BLEDevice::getAdvertising();
  adv->setAdvertisementType(ADV_TYPE_NONCONN_IND);
}

void iniciarRecepcaoSpike() {
  scanSpike = BLEDevice::getScan();

  scanSpike->setAdvertisedDeviceCallbacks(
    new SpikeCallbacks(),
    true
  );

  scanSpike->setActiveScan(false);
  scanSpike->setInterval(100);
  scanSpike->setWindow(99);
  scanSpike->start(0, nullptr, false);
}

void broadcastCanal1(int16_t radio) {
  std::string dados;

  dados.push_back((char)0x97);
  dados.push_back((char)0x03);
  dados.push_back((char)1);
  dados.push_back((char)((3 << 5) | 2));
  dados.push_back((char)(radio & 0xFF));
  dados.push_back((char)((radio >> 8) & 0xFF));

  BLEAdvertisementData pacote;
  pacote.setManufacturerData(dados);

  adv->stop();
  adv->setAdvertisementData(pacote);
  adv->start();
}

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
}

void contarEncoder1() {
  encoder1++;
}

void contarEncoder2() {
  encoder2++;
}

void aplicarMotores(int velocidadeEsquerda, int velocidadeDireita) {
  velocidadeEsquerda = constrain(velocidadeEsquerda, -100, 100);
  velocidadeDireita = constrain(velocidadeDireita, -100, 100);

  int pwmE = map(abs(velocidadeEsquerda), 0, 100, 0, 255);
  int pwmD = map(abs(velocidadeDireita), 0, 100, 0, 255);

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

void run(double cm, int velocidade) {
  offBreak();

  if (cm == 0) {
    brakeRodas();
    return;
  }

  velocidade = constrain(velocidade, VEL_MIN_RUN, 100);

  long inicioE = encoder1;
  long inicioD = encoder2;

  double distanciaMM = fabs(cm) * 10.0;
  double circRoda = PI * DIAMETRO_RODA;

  long pulsosAlvo =
    (long)((distanciaMM / circRoda) * PULSOS_POR_VOLTA);

  if (pulsosAlvo < 1) {
    brakeRodas();
    return;
  }

  while (true) {
    long deltaE = labs(encoder1 - inicioE);
    long deltaD = labs(encoder2 - inicioD);

    if (deltaE >= pulsosAlvo && deltaD >= pulsosAlvo) break;

    double progresso =
      ((double)deltaE + (double)deltaD) /
      (2.0 * pulsosAlvo);

    progresso = min(progresso, 1.0);

    double percorridoCM = fabs(cm) * progresso;
    double restanteCM = fabs(cm) - percorridoCM;

    double fatorAceleracao =
      min(percorridoCM / RAMPA_RUN_CM, 1.0);

    double fatorDesaceleracao =
      min(restanteCM / RAMPA_RUN_CM, 1.0);

    double fator =
      min(fatorAceleracao, fatorDesaceleracao);

    int velocidadeAtual =
      VEL_MIN_RUN +
      (int)((velocidade - VEL_MIN_RUN) * fator);

    velocidadeAtual =
      constrain(
        velocidadeAtual,
        VEL_MIN_RUN,
        velocidade
      );

    long erroEncoder = deltaE - deltaD;
    int correcao =
      (int)(erroEncoder * KP_ALINHAMENTO);

    int velE = velocidadeAtual - correcao;
    int velD = velocidadeAtual + correcao;

    velE = constrain(velE, VEL_MIN_RUN, velocidade);
    velD = constrain(velD, VEL_MIN_RUN, velocidade);

    if (deltaE >= pulsosAlvo) velE = 0;
    if (deltaD >= pulsosAlvo) velD = 0;

    if (cm > 0) {
      aplicarMotores(velE, velD);
    } else {
      aplicarMotores(-velE, -velD);
    }

    delay(10);
  }

  brakeRodas();
}

void curva(double raioMM, double graus, uint8_t velocidade) {
  offBreak();

  if (graus == 0) {
    brakeRodas();
    return;
  }

  double metadeEixos = ENTRE_EIXOS / 2.0;

  if (raioMM <= metadeEixos) {
    brakeRodas();
    return;
  }

  velocidade = constrain(velocidade, 1, 100);

  long inicioE = encoder1;
  long inicioD = encoder2;

  double angulo = fabs(graus);
  double raioInterno = raioMM - metadeEixos;
  double raioExterno = raioMM + metadeEixos;

  double distanciaInterna =
    2.0 * PI * raioInterno * angulo / 360.0;

  double distanciaExterna =
    2.0 * PI * raioExterno * angulo / 360.0;

  double circRoda = PI * DIAMETRO_RODA;

  long pulsosInterno =
    (long)(
      (distanciaInterna / circRoda) *
      PULSOS_POR_VOLTA
    );

  long pulsosExterno =
    (long)(
      (distanciaExterna / circRoda) *
      PULSOS_POR_VOLTA
    );

  pulsosInterno = max(pulsosInterno, 1L);
  pulsosExterno = max(pulsosExterno, 1L);

  double proporcao =
    distanciaInterna / distanciaExterna;

  double potenciaInterna =
    velocidade * proporcao;

  long alvoE;
  long alvoD;

  double potenciaBaseE;
  double potenciaBaseD;

  if (graus > 0) {
    alvoE = pulsosInterno;
    alvoD = pulsosExterno;

    potenciaBaseE = potenciaInterna;
    potenciaBaseD = velocidade;
  } else {
    alvoE = pulsosExterno;
    alvoD = pulsosInterno;

    potenciaBaseE = velocidade;
    potenciaBaseD = potenciaInterna;
  }

  while (true) {
    long deltaE = labs(encoder1 - inicioE);
    long deltaD = labs(encoder2 - inicioD);

    double progressoE =
      (double)deltaE / (double)alvoE;

    double progressoD =
      (double)deltaD / (double)alvoD;

    if (
      (progressoE >= 1.0 && progressoD >= 0.98) ||
      (progressoD >= 1.0 && progressoE >= 0.98)
    ) {
      break;
    }

    double erro =
      progressoE - progressoD;

    const double KP_CURVA = 30.0;

    int correcao =
      (int)(erro * KP_CURVA);

    int velE =
      (int)potenciaBaseE - correcao;

    int velD =
      (int)potenciaBaseD + correcao;

    velE = constrain(velE, 0, velocidade);
    velD = constrain(velD, 0, velocidade);

    if (progressoE >= 1.0) velE = 0;
    if (progressoD >= 1.0) velD = 0;

    aplicarMotores(velE, velD);

    delay(10);
  }

  brakeRodas();
}

static bool rodaEAtiva = false;
static bool rodaDAtiva = false;

static long inicioMotorE = 0;
static long inicioMotorD = 0;

static long alvoMotorE = 0;
static long alvoMotorD = 0;

static int comandoMotorE = 0;
static int comandoMotorD = 0;

static long cmParaPulsos(double cm) {
  double distanciaMM = fabs(cm) * 10.0;
  double circRoda = PI * DIAMETRO_RODA;

  return (long)(
    (distanciaMM / circRoda) *
    PULSOS_POR_VOLTA
  );
}

void motorE(double cm, int potencia) {
  potencia = constrain(abs(potencia), 0, 100);

  if (cm == 0 || potencia == 0) {
    rodaEAtiva = false;
    comandoMotorE = 0;
    return;
  }

  inicioMotorE = encoder1;
  alvoMotorE = cmParaPulsos(cm);
  comandoMotorE = cm > 0 ? potencia : -potencia;
  rodaEAtiva = true;
}

void motorD(double cm, int potencia) {
  potencia = constrain(abs(potencia), 0, 100);

  if (cm == 0 || potencia == 0) {
    rodaDAtiva = false;
    comandoMotorD = 0;
    return;
  }

  inicioMotorD = encoder2;
  alvoMotorD = cmParaPulsos(cm);
  comandoMotorD = cm > 0 ? potencia : -potencia;
  rodaDAtiva = true;
}

void atualizarMotoresRodas() {
  int velE = 0;
  int velD = 0;

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

  digitalWrite(REN1, HIGH);
  digitalWrite(LEN1, HIGH);
  digitalWrite(REN2, HIGH);
  digitalWrite(LEN2, HIGH);

  aplicarMotores(velE, velD);

  if (!rodaEAtiva && !rodaDAtiva) {
    brakeRodas();
  }
}

bool rodasMovendo() {
  return rodaEAtiva || rodaDAtiva;
}

double getAnguloSpike() {
  return (double)anguloSpikeX10 / 10.0;
}

double erroAngular(double alvo, double atual) {
  double erro =
    fmod(alvo - atual, 360.0);

  if (erro > 180.0) erro -= 360.0;
  if (erro < -180.0) erro += 360.0;

  return erro;
}

void setPosition(double alvo, uint8_t velocidade) {
  velocidade =
    constrain(
      velocidade,
      VEL_MIN_GIRO,
      100
    );

  if (!spikeRecebido) {
    SerialBT.println("ERRO: SEM DADOS DO SPIKE");
    brakeRodas();
    return;
  }

  if (millis() - ultimoSpike > 500) {
    SerialBT.println("ERRO: SPIKE SEM SINAL");
    brakeRodas();
    return;
  }

  offBreak();

  unsigned long inicio = millis();

  while (true) {
    double atual = getAnguloSpike();
    double erro = erroAngular(alvo, atual);
    double restante = fabs(erro);

    if (restante <= TOLERANCIA_GIRO) break;

    int velAtual;

    if (restante >= 30.0) {
      velAtual = velocidade;
    } else {
      double fator =
        restante / 30.0;

      velAtual =
        VEL_MIN_GIRO +
        (int)(
          (velocidade - VEL_MIN_GIRO) *
          fator
        );

      velAtual =
        constrain(
          velAtual,
          VEL_MIN_GIRO,
          velocidade
        );
    }

    // positivo = horário
    if (erro > 0) {
      aplicarMotores(
        velAtual,
        -velAtual
      );
    } else {
      aplicarMotores(
        -velAtual,
        velAtual
      );
    }

    if (millis() - ultimoSpike > 500) {
      SerialBT.println("ERRO: SPIKE SEM SINAL");
      break;
    }

    if (millis() - inicio > TIMEOUT_GIRO_MS) {
      SerialBT.println("TIMEOUT GIRO");
      break;
    }

    delay(1);
  }

  brakeRodas();

  SerialBT.print("POSICAO: ");
  SerialBT.println(getAnguloSpike());
}

void giroRelativo(double graus, uint8_t velocidade) {

  if (!spikeRecebido) {
    SerialBT.println("ERRO: SEM SPIKE");
    brakeRodas();
    return;
  }

  if (graus == 0) {
    brakeRodas();
    return;
  }

  velocidade = constrain(
    velocidade,
    VEL_MIN_GIRO,
    100
  );

  double alvo =
    getAnguloSpike() + graus;

  offBreak();

  unsigned long inicioTempo = millis();

  // timeout proporcional ao tamanho do giro
  unsigned long timeout =
    max(
      (unsigned long)TIMEOUT_GIRO_MS,
      (unsigned long)(fabs(graus) * 30.0)
    );

  while (true) {

    double atual =
      getAnguloSpike();

    // NÃO usa erroAngular aqui
    double erro =
      alvo - atual;

    double restante =
      fabs(erro);

    if (restante <= TOLERANCIA_GIRO) {
      break;
    }

    int velAtual;

    if (restante >= 30.0) {

      velAtual = velocidade;

    } else {

      double fator =
        restante / 30.0;

      velAtual =
        VEL_MIN_GIRO +
        (int)(
          (velocidade - VEL_MIN_GIRO)
          * fator
        );

      velAtual =
        constrain(
          velAtual,
          VEL_MIN_GIRO,
          velocidade
        );
    }

    // positivo = horário
    if (erro > 0) {

      aplicarMotores(
        velAtual,
        -velAtual
      );

    } else {

      aplicarMotores(
        -velAtual,
        velAtual
      );
    }

    if (millis() - ultimoSpike > 500) {
      SerialBT.println("ERRO: SPIKE SEM SINAL");
      break;
    }

    if (millis() - inicioTempo > timeout) {
      SerialBT.println("TIMEOUT GIRO");
      break;
    }

    delay(1);
  }

  brakeRodas();
}

bool esperarSpike(unsigned long timeout) {
  unsigned long inicio = millis();

  while (!spikeRecebido) {
    if (millis() - inicio >= timeout) {
      SerialBT.println("ERRO: SPIKE NAO ENCONTRADO");
      return false;
    }

    delay(10);
  }

  SerialBT.println("SPIKE CONECTADO");
  return true;
}

void broadcastCanal(uint8_t canal, int16_t valor) {
  std::string dados;

  dados.push_back((char)0x97);
  dados.push_back((char)0x03);
  dados.push_back((char)canal);

  dados.push_back((char)((3 << 5) | 2));

  dados.push_back((char)(valor & 0xFF));
  dados.push_back((char)((valor >> 8) & 0xFF));

  BLEAdvertisementData pacote;
  pacote.setManufacturerData(dados);

  adv->stop();
  adv->setAdvertisementData(pacote);
  adv->start();
}

const char* ssid = "sua-rede";
const char* password = "sua-senha-wifi";

void setupOTA(const char* hostname) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.onStart([]() {
    Serial.println("OTA iniciado");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA finalizado");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro OTA [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Falha de autenticacao");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha ao iniciar");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha de conexao");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha de recebimento");
    else if (error == OTA_END_ERROR) Serial.println("Falha ao finalizar");
  });

  ArduinoOTA.begin();
  Serial.println(WiFi.localIP());
}

void otaTask(void *param) {
  (void)param;

  while (true) {
    ArduinoOTA.handle();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}