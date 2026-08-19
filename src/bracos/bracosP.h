#pragma once
#include <Arduino.h>

class Servo;

#define SERVOE 23
#define SERVOD 13

#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500
#define SERVO_MAX_GRAUS 300.0f

extern Servo servoE;
extern Servo servoD;

void iniciarServos();
void setServoE(float graus);
void setServoD(float graus);
void setTesteE(float graus, uint8_t potencia);
void setTesteD(float graus, uint8_t potencia);

// ---------- BTS 3 (Direita) ----------
#define D_RPWM 25
#define D_LPWM 26
#define D_REN  27
#define D_LEN  14

// ---------- BTS 4 (Esquerda) ----------
#define E_RPWM 32
#define E_LPWM 33
#define E_REN  18
#define E_LEN  19

// ---------- Encoders ----------
#define ENC_D_A 21
#define ENC_D_B 22
#define ENC_E_A 34
#define ENC_E_B 35

// ---------- Calibração (medida: 10 voltas = 19183 pulsos) ----------
#define PULSOS_POR_VOLTA 1918.3f

// ---------- PID ----------
#define KP 2.2f
#define KI 0.02f
#define KD 0.15f

// ---------- Limites de movimento ----------
#define PWM_MAX           220   // velocidade máxima
#define RAMPA_PASSO       4     // variação máxima de PWM por ciclo (suaviza aceleração/frenagem)
#define TOLERANCIA_PULSOS 5     // erro aceitável
#define CICLO_MS          10    // período do loop de controle
#define INTERVALO_DEBUG   500   // intervalo para prints de debug

// ---------- Brake / Hold (estilo FTC ZeroPowerBehavior.BRAKE) ----------
#define MARGEM_HOLD_GRAUS    3.0f
#define SAIDA_HOLD_GRAUS     8.0f   // tolerância do hold
#define POTENCIA_HOLD_PCT    35
#define TIMEOUT_MOVIMENTO_MS 5000

// ---------- Encoders (contadores globais) ----------
extern volatile long pulsosDireita;
extern volatile long pulsosEsquerda;

// ---------- Controle por Serial / modo console ----------
extern bool modoConsole;
void processarSerial();
void processarComandoConsole(String comando);

// ================================================
// Classe Motor: PID + rampa + limite de velocidade + Brake/Hold
// ================================================
class Motor {
  public:
    enum EstadoMotor {
      EST_IDLE,    // parado, sem comando ativo
      EST_MOVING,  // executando PID rumo ao alvo
      EST_HOLD     // chegou no alvo: segurando posição com brake (baixo consumo)
    };

    Motor(uint8_t rpwm, uint8_t lpwm, uint8_t ren, uint8_t len, volatile long *encoder, const char *nome, bool inverter = false);

    void begin();
    void goTo(float graus, uint8_t potencia);
    bool isMoving();
    bool isHolding();
    void abortar();
    void update();

  private:
    uint8_t _rpwm, _lpwm, _ren, _len;
    volatile long *_encoder;
    const char *_nome;
    bool _inverter;

    long _alvo = 0;
    long _erroAnterior = 0;
    float _integral = 0;
    int _pwmAtual = 0;
    int _pwmDesejado = 0;
    int _pwmMaxAtual = PWM_MAX;
    EstadoMotor _estado = EST_IDLE;
    unsigned long _ultimoUpdate = 0;
    unsigned long _ultimoDebug = 0;

    static int potenciaParaPWM(uint8_t potencia);
    void aplicarPWM(int pwm);
    void aplicarBrake();
    void printStatus(bool inicial);
    void printStatusHold();
};

// ---------- Objetos globais dos braços ----------
extern Motor bracoDireito;
extern Motor bracoEsquerdo;

// ---------- Funções de alto nível (posição absoluta + potência) ----------
void setMotorD(float graus, uint8_t potencia);
void setMotorE(float graus, uint8_t potencia);

// ---------- Inicialização (chamar no setup do main.cpp) ----------
void iniciarBracos();

// ---------- Roda o PID/Hold dos dois braços; true se algum está em MOVING ----------
bool moving();
