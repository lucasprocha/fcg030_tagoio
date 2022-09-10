#include "FS.h"
#include "SPIFFS.h"
#include <rom/rtc.h>
#include "WiFi.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Ticker.h>
#include <HTTPClient.h>
#include <Update.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SerialFlash.h>
#include <string.h>
#include "defines.h"

extern void listDir(const char * dirname, uint8_t levels);
extern int readFile( const char * path, char *content);
extern String readFile( const char * path);
extern void writeFile(const char * path, const char * message);
extern void appendFile(const char * path, const char * message, bool showLog);
extern int sizeFile( const char * path);
extern void deleteFile(const char * path);
extern boolean existFile(const char *path);

extern void print_reset_reason(RESET_REASON reason);
extern void verbose_print_reset_reason(RESET_REASON reason);
extern void updateFirmware(uint8_t *data, size_t len, const char *server);

extern void LOG(const char *type, const char *tag, const char *fmt, ...);

extern void init_wifi();
extern void wifi_reconnect();

extern void readRfid();
extern void getTagsList();

extern boolean reconnect_mqtt();
extern void send_inputs_to_broker(const char *topic, double inputs);
extern void send_inputs_to_broker(const char *topic, float inputs);
extern int init_mqtt();
extern void start_mqtt();

extern void send_to_mpc_i2c();
extern uint8_t read_input_expand_i2c();
extern int init_io_expand_i2c();

extern void memory_extern_init();
extern uint16_t memory_extern_number_of_files();
extern void memory_extern_send_to_server();
extern void memory_extern_write(String content);

extern void otaCheck();

void IRAM_ATTR calculatePulses1();
void IRAM_ATTR calculatePulses2();
void IRAM_ATTR calculateFlows();

AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
Ticker ticker;
Ticker wifiCheck;
Ticker smartConfigCheck;
Ticker smartConfigConnectCheck;
HTTPClient client;
WiFiUDP ntpUDP; // Declaração do Protocolo UDP
NTPClient timeClient(ntpUDP, servidorNTP, fusoHorario, taxaDeAtualizacao);
MFRC522 rfid(SPI_SS_PIN, SPI_RST_PIN);
hw_timer_t * timer = NULL;

uint8_t connect_try = 0;
bool isSmartConfigOn = false;

float input_counter = 0;
float analog_counter = 0;
float second_counter = 0;
float lastReconnectAttempt = 0;
float otaNextCheck = 0;

uint8_t old_input = 0;
unsigned int pulses1 = 0, pulses2 = 0;
float flow1 = 0, flow2 = 0;
double totalFlow1 = 0, totalFlow2 = 0;
double instFlow1 = 0, instFlow2 = 0;
char temp = 0;
uint8_t input;
int analog_inputs[2];

int sendFlowIn1 = FLOW_TIMEOUT_S, sendFlowIn2 = FLOW_TIMEOUT_S;

float totalLength;
float currentLength = 0;

int I2C_ERROR = 0;

String calibrationFactorStr;
String calibrationFactorStr2;

bool rfidIsAllowed = false;
bool useFlow1 = false;
bool useFlow2 = false;
int com_esp = 0; //lucas

uint16_t curFileOnMemory = 0;

void setup()
{

  Serial.begin(115200);
  while (!Serial);
 
  pinMode(DIG_IN_8, INPUT);
  pinMode(DIG_IN_9, INPUT);
  pinMode(COM_PRODUCT_ESP, OUTPUT);
  pinMode(BUZZER_ESP, OUTPUT);
  pinMode(DISP_ESP, OUTPUT);

  digitalWrite(DISP_ESP, HIGH);
  digitalWrite(COM_PRODUCT_ESP, HIGH);  // lucas inicializaçao do com_product_esp com 1 ( habilita o funcionanmento normal da maquina
  
  Serial.print("Dealay0");
  delay(5000);
  Serial.print("Dealay1");
  
  SPI.begin();
  rfid.PCD_Init();
  digitalWrite(COM_PRODUCT_ESP, true);  // lucas inicializaçao do com_product_esp com 1 ( habilita o funcionanmento normal da maquina
  attachInterrupt(digitalPinToInterrupt(DIG_IN_8), calculatePulses1, FALLING);
  attachInterrupt(digitalPinToInterrupt(DIG_IN_9), calculatePulses2, FALLING);

  memory_extern_init();

  curFileOnMemory = memory_extern_number_of_files();
  
  ticker.attach(1, calculateFlows);

  DLOG("RST", "CPU0 reset reason:\n\r");
  print_reset_reason(rtc_get_reset_reason(0));
  verbose_print_reset_reason(rtc_get_reset_reason(0));

  DLOG("RST", "CPU1 reset reason:\n\r");
  print_reset_reason(rtc_get_reset_reason(1));
  verbose_print_reset_reason(rtc_get_reset_reason(1));

  if (!SPIFFS.begin()) {
    ELOG("FS", "SPIFFS mount failed - ");
    return;
  }

  listDir("/", 3);

  I2C_ERROR = init_io_expand_i2c();

  init_wifi();
  wifiCheck.attach(30, wifi_reconnect);

  timeClient.begin();
  delay(100);
  timeClient.update();
  delay(100);
  DLOG("NTP", "Horas atuais: %s:%s\r\n", String(timeClient.getHours()).c_str(), String(timeClient.getMinutes()).c_str());

  String str = readFile(CONFIG_FOLDER CONFIG_USE_RFID);
  rfidIsAllowed = str == "true" ? 1 : 0;

  str = readFile(CONFIG_FOLDER CONFIG_FLOW_1);
  useFlow1 = str == "true" ? 1 : 0;

  str = readFile(CONFIG_FOLDER CONFIG_FLOW_2);
  useFlow2 = str == "true" ? 1 : 0;

#if USE_RFID
  if (rfidIsAllowed) {
    getTagsList();
  }
#endif

  calibrationFactorStr = readFile(CONFIG_FOLDER CONFIG_CALIBRATION);
  calibrationFactorStr2 = readFile(CONFIG_FOLDER CONFIG_CALIBRATION2);

  init_mqtt();
}

void loop() {
  float now = millis();

  if (now > otaNextCheck && WiFi.status() == WL_CONNECTED) {
    otaNextCheck = now + OTA_CHECK_INTERVAL;
#if ALLOW_OKTA
    otaCheck();
#endif
  }

  if (now - input_counter > VERIFY_INPUT_INTERVAL_MS || now < input_counter) {
    timeClient.update();
    input_counter = now;
    input = read_input_expand_i2c();
    if (old_input != input && WiFi.status() == WL_CONNECTED) {
      old_input = input;
      send_inputs_to_broker("inputs", input);
      send_inputs_to_broker("ps_pin", digitalRead(PS_2) << 1 | digitalRead(PS_1));
    }
  }

  if (now - analog_counter > VERIFY_ANALOG_INTERVAL_MS || now < analog_counter) {
    analog_counter = now;
#if SIMULATE_DATA
    analog_inputs[0] = (int)(millis()) % 4096;
    analog_inputs[1] = (int)(millis() * 10) % 4096;
    send_inputs_to_broker("analog/ana_in_4", analog_inputs[0]);
    send_inputs_to_broker("analog/ana_in_5", analog_inputs[1]);
#else

    // inserir aqui a condição de enviar somente quando a maquina estiver habilitada.
    // DISP_ESP = 0 testbit (DIG_IN_0 - Enable Geral e DIG_IN_5 Base  ( só liga quando base fica baixo)


    if (digitalRead(DISP_ESP) == LOW && TEST_BIT(input, 6) == 0 && TEST_BIT(input, 0) == 0)
      //if (pin != "disp_esp") // & ((input & B00100001)==33) ) // input binario 00000000 x  00100001
    {

      analog_inputs[0] = analogRead(ANA_IN_4);
      analog_inputs[1] = analogRead(ANA_IN_5);
      instFlow1 = flow1;
      send_inputs_to_broker("instFlow1", instFlow1);
      instFlow2 = flow2;
      send_inputs_to_broker("instFlow2", instFlow2);
      send_inputs_to_broker("analog/ana_in_4", analog_inputs[0]);
      send_inputs_to_broker("analog/ana_in_5", analog_inputs[1]);

    }
#endif
  }

#if USE_RFID
  if (rfidIsAllowed) {
    rfid.PCD_Init();     // Init MFRC522
    readRfid();
  }
#endif

  if (WiFi.status() == WL_CONNECTED) {
    if (!mqttClient.connected()) {
      if (now - lastReconnectAttempt > BROKER_RECONNECT_TIME) {
        lastReconnectAttempt = now;
        reconnect_mqtt();
      }
    } else {
      // Client connected
      mqttClient.loop();
      memory_extern_send_to_server();
    }
  }
}

void IRAM_ATTR calculatePulses1() {
  pulses1++;
}

void IRAM_ATTR calculatePulses2() {
  pulses2++;
}

void IRAM_ATTR calculateFlow1() {
#if SIMULATE_DATA
  temp++;
  if (temp < 30) {
    flow1 = (millis() % 20) / 5.5;
    totalFlow += flow1 / 60;
  }
  if (temp == 60) {
    temp = 0;
    send_inputs_to_broker("totalFlow1", totalFlow);
  }
#else
  if (calibrationFactorStr != "") {
    flow1 = pulses1 * calibrationFactorStr.toFloat();
    totalFlow1 += flow1 / 60;
    sendFlowIn1--;
    pulses1 = 0;
    if (sendFlowIn1 == 0) {
      sendFlowIn1 = FLOW_TIMEOUT_S;
      if (WiFi.status() == WL_CONNECTED) {
        send_inputs_to_broker("totalFlow1", totalFlow1);
      } else {
        memory_extern_write("totalFlow1;"+String(timeClient.getEpochTime())+String(";")+String(totalFlow1));
      }
    }
  }
#endif
}

void IRAM_ATTR calculateFlow2() {
#if SIMULATE_DATA
  temp++;
  if (temp < 30) {
    flow2 = (millis() % 20) / 5.5;
    totalFlow2 += flow2 / 60;
  }
  if (temp == 60) {
    temp = 0;
    send_inputs_to_broker("totalFlow2", totalFlow2);
  }
#else
  if (calibrationFactorStr2 != "") {
    flow2 = pulses2 * calibrationFactorStr2.toFloat();
    totalFlow2 += flow2 / 60;
    sendFlowIn2--;
    pulses2 = 0;
    if (sendFlowIn2 == 0) {
      sendFlowIn2 = FLOW_TIMEOUT_S;
      if (WiFi.status() == WL_CONNECTED) {
        send_inputs_to_broker("totalFlow2", totalFlow2);
      } else {
        memory_extern_write("totalFlow2;"+String(timeClient.getEpochTime())+String(";")+String(totalFlow2));
      }
    }
  }
#endif
}

void IRAM_ATTR calculateFlows() {
  if (useFlow1) calculateFlow1();
  if (useFlow2) calculateFlow2();
}
