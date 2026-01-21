#include <WiFi.h>
#include <esp_now.h>
#include "esp_task_wdt.h"

// ===================
//    DEFINIÇÕES
// ===================
#define LED_PIN 8       // LED onboard ESP32-C3 mini
#define MOTOR_PIN 20    // Transistor -> motor vibrador

#define MOTOR_PWM_FREQ 5000
#define MOTOR_PWM_RES  8   // 0–255

// ===================
//    VARIÁVEIS
// ===================
typedef struct struct_message {
  char command[32];
} struct_message;

struct_message msg;

// Ciclo de alarme
bool motorVibrando = false;
bool motorOnFase = false;
unsigned long motorTimer = 0;

// LED segue motor
bool motorAtivo = false;

// Ações curtas
bool acaoTemporariaAtiva = false;
unsigned long acaoTimer = 0;
int acaoDuracao = 0;


// ===================
//  FUNÇÕES MOTOR & LED
// ===================
void atualizarMotorELed(bool ativo) {
  digitalWrite(LED_PIN, ativo ? LOW : HIGH);
  motorAtivo = ativo;
}

void motorPWM(int intensidade) {
  ledcWrite(MOTOR_PIN, intensidade);  // 0–255
}


// ===================
//     PISCAR PARA DEF
// ===================
void piscarLed(int vezes) {
  for (int i = 0; i < vezes; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(150);
    digitalWrite(LED_PIN, HIGH);
    delay(150);
  }
}


// ==========================
//   ALARME X DEF (NOVO!)
// ==========================
void executarAlarmeDef(int numeroAlarme) {

  // Bloqueia ciclo
  motorVibrando = false;
  acaoTemporariaAtiva = true;
  acaoTimer = millis();
  acaoDuracao = 300;  // 2 vibrações curtas (total ~600ms)

  Serial.printf("Executando alarme %d DEF\n", numeroAlarme);

  // 2 vibrações curtas e fracas
  for (int i = 0; i < 2; i++) {
    motorPWM(120);
    atualizarMotorELed(true);
    delay(100);
    motorPWM(0);
    atualizarMotorELed(false);
    delay(150);
  }

  // LED pisca o número do alarme
  piscarLed(numeroAlarme);

  // Finaliza
  motorPWM(0);
  atualizarMotorELed(false);
  acaoTemporariaAtiva = false;
}


// ===================
//   AÇÕES CURTAS
// ===================
void executarResetComum() {
  acaoTemporariaAtiva = true;
  acaoTimer = millis();
  acaoDuracao = 250;

  motorPWM(120);
  atualizarMotorELed(true);
}

void executarResetAll() {
  acaoTemporariaAtiva = true;
  acaoTimer = millis();
  acaoDuracao = 500;

  motorPWM(255);
  atualizarMotorELed(true);
}

void executarErroDef() {
  acaoTemporariaAtiva = true;
  acaoTimer = millis();
  acaoDuracao = 250;

  motorPWM(255);
  atualizarMotorELed(true);
}


// ===================
//   CICLO 3s ON/OFF
// ===================
void atualizarCicloMotor() {
  if (!motorVibrando || acaoTemporariaAtiva) return;

  unsigned long agora = millis();

  if (motorOnFase) {
    if (agora - motorTimer >= 3000) {
      motorOnFase = false;
      motorPWM(0);
      atualizarMotorELed(false);
      motorTimer = agora;
    }
  } else {
    if (agora - motorTimer >= 3000) {
      motorOnFase = true;
      motorPWM(255);
      atualizarMotorELed(true);
      motorTimer = agora;
    }
  }
}


// ===================
//  AÇÕES TEMPORÁRIAS
// ===================
void atualizarAcoesTemporarias() {
  if (!acaoTemporariaAtiva) return;

  if (millis() - acaoTimer >= acaoDuracao) {
    acaoTemporariaAtiva = false;
    motorPWM(0);
    atualizarMotorELed(false);
  }
}


// ===================
//     CALLBACK ESP-NOW
// ===================
void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
  memcpy(&msg, incomingData, sizeof(msg));
  String comando = String(msg.command);
  comando.trim();

  Serial.print("Comando recebido: ");
  Serial.println(comando);

  if (acaoTemporariaAtiva) {
    Serial.println("Ação temporária ativa — ignorando comandos.");
    return;
  }

  // ---------------------------
  // Detectar "alarmeXdef"
  // ---------------------------
  if (comando.startsWith("alarme") && comando.endsWith("def")) {
    int numero = comando.substring(6, comando.length() - 3).toInt();
    if (numero >= 1 && numero <= 6) {
      executarAlarmeDef(numero);
      return;
    }
  }

  // ---- comandos normais ----
  if (comando.equalsIgnoreCase("alarmetocando")) {
    Serial.println("Ciclo 3s ON / 3s OFF iniciado.");

    motorVibrando = true;
    motorOnFase = true;
    motorTimer = millis();

    motorPWM(255);
    atualizarMotorELed(true);
  }

  else if (comando.equalsIgnoreCase("paredevibrar")) {
    Serial.println("Motor desligado.");
    motorVibrando = false;
    motorPWM(0);
    atualizarMotorELed(false);
  }

  else if (comando.equalsIgnoreCase("resetalarmecomum")) {
    Serial.println("Reset comum — vibração fraca 1s.");
    motorVibrando = false;
    executarResetComum();
  }

  else if (comando.equalsIgnoreCase("resetalarmeall")) {
    Serial.println("Reset ALL — vibração forte 2s.");
    motorVibrando = false;
    executarResetAll();
  }

  else if (comando.equalsIgnoreCase("errodef")) {
    Serial.println("Erro DEF — vibração forte e curta 1s.");
    motorVibrando = false;
    executarErroDef();
  }

  else {
    Serial.println("Comando desconhecido!");
  }
}


// ===================
//       SETUP
// ===================
void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // PWM no ESP32-C3
  ledcAttach(MOTOR_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  motorPWM(0);

  // ESP-NOW
  WiFi.mode(WIFI_STA);
  Serial.println("Inicializando ESP-NOW...");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Aguardando comandos...");

  // Watchdog (API nova)
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = 5000,
      .idle_core_mask = (1 << 0),
      .trigger_panic = true
  };

  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);
}


// ===================
//       LOOP
// ===================
void loop() {
  // Mantém o watchdog vivo
  esp_task_wdt_reset();

  // Atualiza ações temporárias (reset comum, reset all, erro def)
  atualizarAcoesTemporarias();

  // Atualiza ciclo de 3s ON / 3s OFF do alarme
  atualizarCicloMotor();

  // Nada bloqueante no loop — tudo executado por timers
}

