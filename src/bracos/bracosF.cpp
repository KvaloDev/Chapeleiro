#include "bracosP.h"
#include <ESP32Servo.h>

// quadratura
static const int8_t TABELA_QUAD[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

Servo servoE;
Servo servoD;


// Encoders

volatile long pulsosDireita = 0;
volatile long pulsosEsquerda = 0;
static volatile uint8_t _estadoD = 0;
static volatile uint8_t _estadoE = 0;

void IRAM_ATTR isrEncoderDireita() {
  _estadoD <<= 2;
  if (digitalRead(ENC_D_A)) _estadoD |= 0b10;
  if (digitalRead(ENC_D_B)) _estadoD |= 0b01;
  pulsosDireita += TABELA_QUAD[_estadoD & 0x0F];
}

void IRAM_ATTR isrEncoderEsquerda() {
  _estadoE <<= 2;
  if (digitalRead(ENC_E_A)) _estadoE |= 0b10;
  if (digitalRead(ENC_E_B)) _estadoE |= 0b01;
  pulsosEsquerda -= TABELA_QUAD[_estadoE & 0x0F];
}

// ================================================
// Classe Motor
// ================================================
Motor::Motor(uint8_t rpwm, uint8_t lpwm, uint8_t ren, uint8_t len, volatile long *encoder, const char *nome, bool inverter)
  : _rpwm(rpwm), _lpwm(lpwm), _ren(ren), _len(len), _encoder(encoder), _nome(nome), _inverter(inverter) {}


static int grausParaMicros(float graus) {
  graus = constrain(graus, 0.0f, SERVO_MAX_GRAUS);

  return (int)(
    SERVO_MIN_US +
    (graus / SERVO_MAX_GRAUS) *
    (SERVO_MAX_US - SERVO_MIN_US)
  );
}

void iniciarServos() {
  servoE.setPeriodHertz(50);
  servoD.setPeriodHertz(50);

  servoE.attach(SERVOE, SERVO_MIN_US, SERVO_MAX_US);
  servoD.attach(SERVOD, SERVO_MIN_US, SERVO_MAX_US);
}

void setServoE(float graus) {
  graus = constrain(graus, 0.0f, 140.0f);
  servoE.writeMicroseconds(grausParaMicros(graus));
}

void setServoD(float graus) {
  graus = constrain(graus, 0.0f, 130.0f);
  float grausInvertidos = 130.0f - graus;
  servoD.writeMicroseconds(grausParaMicros(grausInvertidos));
}


void Motor::begin() {
  pinMode(_rpwm, OUTPUT);
  pinMode(_lpwm, OUTPUT);
  pinMode(_ren, OUTPUT);
  pinMode(_len, OUTPUT);
  digitalWrite(_ren, HIGH);
  digitalWrite(_len, HIGH);
}

// goTo recebe a potência (0-100%) desejada para o movimento.
// Qualquer chamada de goTo cancela automaticamente um hold em andamento.
void Motor::goTo(float graus, uint8_t potencia) {
  _alvo = (long)((graus / 360.0f) * PULSOS_POR_VOLTA);

  _pwmMaxAtual = potenciaParaPWM(potencia);

  _erroAnterior = 0;
  _integral = 0;
  _pwmAtual = 0;
  _ultimoDebug = 0;

  _estado = EST_MOVING;
  printStatus(true);
}

// true somente durante o deslocamento ativo (PID rodando)
bool Motor::isMoving() {
  return _estado == EST_MOVING;
}

bool Motor::isHolding() {
  return _estado == EST_HOLD;
}

// Aborta o movimento atual com segurança: aplica o brake e volta pra IDLE.
// Usado pelo timeout de setMotorD/setMotorE quando o motor não chega no alvo.
void Motor::abortar() {
  aplicarPWM(0);

  _pwmAtual = 0;
  _pwmDesejado = 0;
  _integral = 0;
  _estado = EST_IDLE;
}

void Motor::update() {
  unsigned long agora = millis();
  if (agora - _ultimoUpdate < CICLO_MS) return;
  _ultimoUpdate = agora;

  long posicao = *_encoder;
  long erro = _alvo - posicao;

  switch (_estado) {

    case EST_MOVING: {
      if (abs(erro) <= TOLERANCIA_PULSOS) {
        // Chegou: entra em modo BRAKE/HOLD (estilo FTC ZeroPowerBehavior.BRAKE)
        _estado = EST_HOLD;
        _pwmAtual = 0;
        _pwmDesejado = 0;
        aplicarBrake();
        printStatus(false);
        Serial.println();
        return;
      }

      _integral += erro;
      _integral = constrain(_integral, -3000.0f, 3000.0f); // anti-windup
      float derivada = erro - _erroAnterior;
      _erroAnterior = erro;

      float saida = (KP * erro) + (KI * _integral) + (KD * derivada);
      _pwmDesejado = constrain((int)saida, -_pwmMaxAtual, _pwmMaxAtual);

      // Rampa: aproxima o PWM atual do desejado aos poucos (sem trancos)
      if (_pwmAtual < _pwmDesejado) {
        _pwmAtual = min(_pwmAtual + RAMPA_PASSO, _pwmDesejado);
      } else if (_pwmAtual > _pwmDesejado) {
        _pwmAtual = max(_pwmAtual - RAMPA_PASSO, _pwmDesejado);
      }

      aplicarPWM(_pwmAtual);

      if (agora - _ultimoDebug >= INTERVALO_DEBUG) {
        _ultimoDebug = agora;
        printStatus(false);
      }
      break;
    }

    case EST_HOLD: {

  // HOLD ATIVO:
  // continua usando o encoder + PID para lutar contra a gravidade

  _integral += erro;
  _integral = constrain(
    _integral,
    -3000.0f,
    3000.0f
  );

  float derivada =
    erro - _erroAnterior;

  _erroAnterior = erro;

  float saida =
    (KP * erro) +
    (KI * _integral) +
    (KD * derivada);

  int pwmMaxHold =
    potenciaParaPWM(POTENCIA_HOLD_PCT);

  _pwmDesejado =
    constrain(
      (int)saida,
      -pwmMaxHold,
      pwmMaxHold
    );

  // rampa pra correção não dar tranco
  if (_pwmAtual < _pwmDesejado) {

    _pwmAtual =
      min(
        _pwmAtual + RAMPA_PASSO,
        _pwmDesejado
      );

  }
  else if (_pwmAtual > _pwmDesejado) {

    _pwmAtual =
      max(
        _pwmAtual - RAMPA_PASSO,
        _pwmDesejado
      );
  }

  aplicarPWM(_pwmAtual);

  break;
}

    case EST_IDLE:
    default:
      break;
  }
}

int Motor::potenciaParaPWM(uint8_t potencia) {
  potencia = constrain(potencia, (uint8_t)0, (uint8_t)100);
  return (int)((potencia / 100.0f) * PWM_MAX);
}

void Motor::aplicarPWM(int pwm) {

  if (_inverter) {
    pwm = -pwm;
  }

  if (pwm > 0) {
    analogWrite(_rpwm, pwm);
    analogWrite(_lpwm, 0);
  }
  else if (pwm < 0) {
    analogWrite(_rpwm, 0);
    analogWrite(_lpwm, -pwm);
  }
  else {
    analogWrite(_rpwm, 0);
    analogWrite(_lpwm, 0);
  }
}

void Motor::aplicarBrake() {
  analogWrite(_rpwm, 0);
  analogWrite(_lpwm, 0);
}

void Motor::printStatus(bool inicial) {
  long posicao = *_encoder;
  float grausAtual = (posicao / PULSOS_POR_VOLTA) * 360.0f;
  float grausAlvo = (_alvo / PULSOS_POR_VOLTA) * 360.0f;

  if (inicial) {
    Serial.print("[");
    Serial.print(_nome);
    Serial.print("] COMANDO: ");
    Serial.print(grausAlvo, 1);
    Serial.print("° (");
    Serial.print(_alvo);
    Serial.print(" pulsos) | Potência: ");
    Serial.print((int)((_pwmMaxAtual / (float)PWM_MAX) * 100.0f));
    Serial.println("%");
  } else if (_estado == EST_MOVING) {
    Serial.print("[");
    Serial.print(_nome);
    Serial.print("] Posição: ");
    Serial.print(grausAtual, 1);
    Serial.print("° (");
    Serial.print(posicao);
    Serial.print(" pulsos) | Alvo: ");
    Serial.print(grausAlvo, 1);
    Serial.print("° (");
    Serial.print(_alvo);
    Serial.print(" pulsos) | PWM: ");
    Serial.println(_pwmAtual);
  } else if (_estado == EST_HOLD) {
    Serial.print("[");
    Serial.print(_nome);
    Serial.print("] ✓ CHEGOU em ");
    Serial.print(grausAtual, 1);
    Serial.print("° (");
    Serial.print(posicao);
    Serial.println(" pulsos) | Entrando em HOLD (brake)");
  }
}

void Motor::printStatusHold() {
  long posicao = *_encoder;
  float grausAtual = (posicao / PULSOS_POR_VOLTA) * 360.0f;
  float grausAlvo = (_alvo / PULSOS_POR_VOLTA) * 360.0f;

  Serial.print("[");
  Serial.print(_nome);
  Serial.print("] 🔒 HOLD | Posição: ");
  Serial.print(grausAtual, 1);
  Serial.print("° | Alvo: ");
  Serial.print(grausAlvo, 1);
  Serial.println("°");
}

// ================================================
// Objetos globais dos braços
// ================================================
Motor bracoDireito(D_RPWM, D_LPWM, D_REN, D_LEN, &pulsosDireita, "DIREITO", false);
Motor bracoEsquerdo(E_RPWM, E_LPWM, E_REN, E_LEN, &pulsosEsquerda, "ESQUERDO", true);

// ================================================
// Controle por Serial / modo console
// ================================================
bool modoConsole = false;

void processarSerial() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "0") {
      modoConsole = true;
      Serial.println("\n╔════════════════════════════════════╗");
      Serial.println("║   MODO CONSOLE ATIVADO              ║");
      Serial.println("║   Digite graus para ESQUERDO (E)    ║");
      Serial.println("║   ou DIREITO (D)                    ║");
      Serial.println("║   Exemplo: E 90 ou D -45            ║");
      Serial.println("║   Com potência: E 90 80 (80%)       ║");
      Serial.println("╚════════════════════════════════════╝\n");
    } else if (modoConsole) {
      processarComandoConsole(input);
    }
  }
}

void processarComandoConsole(String comando) {
  // Formato: "E 90" ou "D -45" ou "E 90 80" (com potência)
  int espaco1 = comando.indexOf(' ');

  if (espaco1 == -1) {
    Serial.println("❌ Formato inválido! Use: E 90 ou D -45");
    return;
  }

  char motor = comando.charAt(0);
  String resto = comando.substring(espaco1 + 1);
  resto.trim();

  int espaco2 = resto.indexOf(' ');
  float graus;
  uint8_t potencia = 60; // potência padrão no modo console (60%)

  if (espaco2 == -1) {
    graus = resto.toFloat();
  } else {
    graus = resto.substring(0, espaco2).toFloat();
    potencia = (uint8_t)constrain(resto.substring(espaco2 + 1).toInt(), 0, 100);
  }

  if (motor == 'E' || motor == 'e') {
    Serial.print("→ Movendo ESQUERDO para ");
    Serial.print(graus, 1);
    Serial.print("° com ");
    Serial.print(potencia);
    Serial.println("% de potência");
    setMotorE(graus, potencia);
  } else if (motor == 'D' || motor == 'd') {
    Serial.print("→ Movendo DIREITO para ");
    Serial.print(graus, 1);
    Serial.print("° com ");
    Serial.print(potencia);
    Serial.println("% de potência");
    setMotorD(graus, potencia);
  } else {
    Serial.println("❌ Motor inválido! Use 'E' ou 'D'");
  }
}

// ================================================
// setMotorD / setMotorE: posição absoluta (graus) + potência (0-100%)
// Qualquer chamada cancela automaticamente um hold anterior.
// ================================================
void setMotorD(float graus, uint8_t potencia) {
  bracoDireito.goTo(graus, potencia);
  unsigned long inicio = millis();
  while (bracoDireito.isMoving()) {
    bracoDireito.update();
    bracoEsquerdo.update();
    processarSerial();
    delay(CICLO_MS);

    if (millis() - inicio > TIMEOUT_MOVIMENTO_MS) {
      Serial.println("[DIREITO] ❌ TIMEOUT: motor não chegou no alvo (verifique alimentação/fiação). Abortando movimento.");
      bracoDireito.abortar();
      break;
    }
  }
}

void setMotorE(float graus, uint8_t potencia) {
  bracoEsquerdo.goTo(graus, potencia);
  unsigned long inicio = millis();
  while (bracoEsquerdo.isMoving()) {
    bracoEsquerdo.update();
    bracoDireito.update();
    processarSerial();
    delay(CICLO_MS);

    if (millis() - inicio > TIMEOUT_MOVIMENTO_MS) {
      Serial.println("[ESQUERDO] ❌ TIMEOUT: motor não chegou no alvo (verifique alimentação/fiação). Abortando movimento.");
      bracoEsquerdo.abortar();
      break;
    }
  }
}

// ================================================
// Inicialização (chamar no setup do main.cpp)
// ================================================
void iniciarBracos() {
  pinMode(ENC_D_A, INPUT_PULLUP);
  pinMode(ENC_D_B, INPUT_PULLUP);
  pinMode(ENC_E_A, INPUT);
  pinMode(ENC_E_B, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENC_D_A), isrEncoderDireita, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_D_B), isrEncoderDireita, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_E_A), isrEncoderEsquerda, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_E_B), isrEncoderEsquerda, CHANGE);

  bracoDireito.begin();
  bracoEsquerdo.begin();

  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  SISTEMA DE BRAÇOS INICIADO         ║");
  Serial.println("║  Modo: POSICIONAMENTO ABSOLUTO      ║");
  Serial.println("║  Brake/Hold: ATIVADO (estilo FTC)   ║");
  Serial.println("║                                      ║");
  Serial.println("║  Digite '0' para MODO CONSOLE       ║");
  Serial.println("╚════════════════════════════════════╝\n");
}

// ================================================
// moving(): roda o PID/Hold dos dois braços e informa
// se algum ainda está em movimento ativo (MOVING).
// Chamar continuamente no loop principal pra o HOLD
// (brake + correção) continuar funcionando mesmo depois
// que setMotorD()/setMotorE() já retornaram.
// ================================================
bool moving() {

  processarSerial();

  // SEMPRE atualiza os dois,
  // inclusive quando já chegaram no alvo
  bracoDireito.update();
  bracoEsquerdo.update();

  if (modoConsole) {
    return false;
  }

  return
    bracoDireito.isMoving() ||
    bracoEsquerdo.isMoving();
}

void offBreak() {
  bracoDireito.abortar();
  bracoEsquerdo.abortar();
}