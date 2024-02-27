#define FW_VERSION "v0.6"
#define MODEL "HUB-L01 1.0"


#define SSID_NOMBRE "HUBM-"
#define CONFIG_USE_CRC32 0 // Esto define si el archivo recibido va a venir con el CRC calculado o no. Si no viene, lo calculo y lo guarod con el CRC valido.

#define WDT_TIMEOUT 200
const int timeZone = -3; // BsAs

#define CANT_MAX_INA 100

#define BUFFER_RECEPCION_ESPNOW 200

#define MAX_DATA_TELEMETRIA_GPS_LIST 5
#define MAX_DATA_TELEMETRIA_INA_LIST 5

#define TINY_GSM_MODEM_SIM7600 
//#define TINY_GSM_MODEM_SIM7000


#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
//#define TINY_GSM_DEBUG Serial

//#define DUMP_AT_COMMANDS

//#define USE_SSL

#define SerialSIM Serial1

#define SD_CS_PIN 15

#define PIN_SIM_RX 26
#define PIN_SIM_TX 25


#define SIM_POWER 17


#define BAT_ENABLE 32

#define PIN_SENSOR_VLIN 36
#define PIN_SENSOR_VBAT 39

#define PIN_POWEROFFBAT 33

// CORRECCION BATERIA
#define CORRECCION_VOLT_BATERIA 0 // mv


#define TIMEOUT_DEAD_INALAMBRICOS 480000L  // 300 seg.  Tiempo en el cual si no recibo transmision de un inalambrico lo tomo como muerto para las estadisticas


// ---------------------  REDES CELULAR   -----------------------

// Movistar
//#define CELL_MOVISTAR_APN "wap.gprs.unifon.com.ar"
#define CELL_MOVISTAR_APN "gm2m.movistar"
#define CELL_MOVISTAR_USER "gm2m"
#define CELL_MOVISTAR_PASS "gm2m"

// Claro

#define CELL_CLARO_APN "iot.claro.com.ar"
#define CELL_CLARO_USER ""
#define CELL_CLARO_PASS ""

// Personal

#define CELL_PERSONAL_APN "datos.personal.com"
#define CELL_PERSONAL_USER "datos"
#define CELL_PERSONAL_PASS "datos"


// -----------  Config MQTT  ------------------------------------


constexpr char THINGSBOARD_SERVER[] = "13.116.94.205";
char TOKEN[20] = "";  //  Se completa co el ID del dispositivo en el setup()

constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;

