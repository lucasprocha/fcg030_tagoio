#ifndef defines_h
#define defines_h
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define SIMULATE_DATA     0
#define USE_RFID          1
#define ALLOW_OKTA        0

#define ENABLE_COLORS_LOG 0
#define ENABLE_LOG_INFO   1
#define ENABLE_LOG_ERROR  1
#define ENABLE_LOG_WARN   1
#define ENABLE_LOG_DEBUG  1
#define ENABLE_LOG_FILE   0

#define DEFAULT_URL           "esp32-fluxo"
#define DEFAULT_USERNAME      "admin"
#define DEFAULT_USERPASSWORD  "admin"

#define WEBPAGE_HTML_FILE "index.html"
#define WEBPAGE_FILE_1    "2.e3e2974d.chunk.js"
#define WEBPAGE_FILE_2    "main.980bb2a6.chunk.js"
#define WEBPAGE_FILE_3    "runtime-main.a90aa7a1.js"
#define WEBPAGE_FILE_4    "main.7bed1a89.chunk.css"
#define WEBPAGE_FOLDER    "/page/"

#define SSID_FILE         "/ssid.txt"
#define PASS_FILE         "/pass.txt"

#define FW_FILE           "/fwVersion.txt"
#define WEB_PAGE_FILE     "/webPageVersion.txt"
#define TAGS_FILE         "/tags.txt"

#define CONFIG_LAT                "lat.txt"
#define CONFIG_LONG               "long.txt"
#define CONFIG_BROKER             "broker.txt"
#define CONFIG_TOKEN              "token.txt"
#define CONFIG_PORT               "port.txt"
#define CONFIG_TAGS_URL           "tags.txt"
#define CONFIG_MODEL              "model.txt"
#define CONFIG_SERIAL             "serial.txt"
#define CONFIG_CALIBRATION        "calibration.txt"
#define CONFIG_CALIBRATION2       "calibration2.txt"
#define CONFIG_CALIBRATION_ANA_4  "calibration_ana4.txt"
#define CONFIG_CALIBRATION_ANA_5  "calibration_ana5.txt"
#define CONFIG_USE_RFID           "useRfid.txt"
#define CONFIG_FLOW_1             "flow1.txt"
#define CONFIG_FLOW_2             "flow2.txt"
#define CONFIG_FOLDER             "/config/"

#define OUTPUT_PIN_STATUS   "/outStatus.txt"

#define MQTT_HOST         "mqtt.tago.io"        // Broker address
#define MQTT_PORT         1883
#define MQTT_USERNAME     "Default"

#define LAST_OTA_SERVER_FILE    "/last_ota.txt"

#define VERIFY_INPUT_INTERVAL_MS     5000
#define VERIFY_ANALOG_INTERVAL_MS    5000
#define BROKER_RECONNECT_TIME        10000
#define FLOW_TIMEOUT_S               30
#define MACHINE_TIMEOUT_S            5 * 60

#define DIG_IN_8         26
#define DIG_IN_9         25
#define ANA_IN_4         35
#define ANA_IN_5         34
#define PS_1              2
#define PS_2             15
#define COM_PRODUCT_ESP   5
#define BUZZER_ESP       17
#define DISP_ESP         16

#define SPI_SS_PIN     4
#define SPI_RST_PIN   15

#define SPI_MEMORY_CS_PIN          39
#define SPI_MEMORY_MAX_FILE_SIZE   64 //bytes

#if ENABLE_LOG_ERROR
#define ELOG(...) LOG("ERROR",  __VA_ARGS__)
#else
#define ELOG(...)
#endif
#if ENABLE_LOG_DEBUG
#define DLOG(...) LOG("DEBUG",  __VA_ARGS__)
#else
#define DLOG(...)
#endif
#if ENABLE_LOG_INFO
#define ILOG(...) LOG("INFO",  __VA_ARGS__)
#else
#define ILOG(...)
#endif
#if ENABLE_LOG_WARN
#define WLOG(...) LOG("WARN",  __VA_ARGS__)
#else
#define WLOG(...)
#endif

#if ENABLE_LOG_FILE
#define FLOG(...) LOG("FILE",  __VA_ARGS__)
#else
#define FLOG(...)
#endif

#define SET_BIT(x, n)    (x |= 1UL << n)
#define CLEAR_BIT(x, n)  (x &= ~(1UL << n))
#define TEST_BIT(x, n)   ((x >> n) & 1U)

#define     OTA_CONTROL_URL     "https://dl.dropboxusercontent.com/s/yaccot2n0g7z9l9/otaControl.txt"
#if SIMULATE_DATA
#define     OTA_CHECK_INTERVAL  1000 * 10 //10s
#else
#define     OTA_CHECK_INTERVAL  1000 * 60 *60 *6 //6h
#endif

const char* servidorNTP = "a.st1.ntp.br"; // Servidor NTP para pesquisar a hora
const int fusoHorario = -10800; // Fuso horário em segundos (-03h = -10800 seg)
const int taxaDeAtualizacao = 1800000; // Taxa de atualização do servidor NTP em milisegundos

#endif    //defines_h
