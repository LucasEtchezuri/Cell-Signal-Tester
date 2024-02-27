// Para que funcione el envio por MQTT de paquetes grandes, modificar el archivo PubSubClient.h de la libreria.
// Modificar en la linea 26
// MQTT_MAX_PACKET_SIZE : Maximum packet size
// #define MQTT_MAX_PACKET_SIZE 10240

// IMPORTANTE
// En el archivo uFire_SHT20.h  de la libreria del SHT21
// se modifico la linea #define TEMPERATURE_DELAY, se le puso el valor 25 ! Que alcanza para medir en una ressolucion de 11 bits
// El valor que traia por defecto era 100ms, lo cual es mucho para un desarollo de bajo consumo.

#include <Arduino.h>
#include <WiFi.h>
#include <U8g2lib.h>
#include "config.h"
#include "estructuras.h"
#include "TinyGsmClient.h"

#include <Preferences.h>

#include <Wire.h>

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// U8G2_SH1106_128X64_NONAME_1_SW_I2C(rotation, clock, data [, reset]) [page buffer, size = 128 bytes]
// U8G2_SH1106_128X64_NONAME_2_SW_I2C(rotation, clock, data [, reset]) [page buffer, size = 256 bytes]
// U8G2_SH1106_128X64_NONAME_F_SW_I2C(rotation, clock, data [, reset]) [full framebuffer, size = 1024 bytes]

uint8_t i2c_2_clk = 27;
uint8_t i2c_2_data = 32;

U8G2_SH1106_128X64_NONAME_1_HW_I2C disp_1(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_1_HW_I2C disp_2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_1_HW_I2C disp_3(U8G2_R0, U8X8_PIN_NONE, i2c_2_clk, i2c_2_data);

// U8G2_SH1106_128X64_NONAME_1_SW_I2C disp_1(U8G2_R0, disp_1_clk, disp_1_data);

// #define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialSIM, Serial);
#ifdef TINY_GSM_MODEM_SIM7000
TinyGsmSim7000 modem(SerialSIM);
#endif
#ifdef TINY_GSM_MODEM_SIM7600
TinyGsmSim7600 modem(SerialSIM);
#endif
#else
#ifdef TINY_GSM_MODEM_SIM7000
TinyGsmSim7000 modem(SerialSIM);
#endif
#ifdef TINY_GSM_MODEM_SIM7600
TinyGsmSim7600 modem(SerialSIM);
#endif
#endif

TinyGsmClient clientSIM(modem);

st_mediciones mediciones;
st_hardwareDetectado hardwareDetectado;
st_configuracion configuracion;
st_status status;

uint32_t lastReconnectAttemptCell = 0;
uint32_t tiempoDesconeccionCELL = 0;
uint32_t tiempoDesconeccionCellNetwork = 0;
uint32_t lastTestRecoveryTransmision = 0;

String modemName = "";
String modemInfo = "";

uint16_t ID = 0;

bool primerInicio = true;

#include "funciones.h"

void setup()
{
  Serial.begin(115200);

  pinMode(SIM_POWER, OUTPUT);

  digitalWrite(SIM_POWER, LOW);

  disp_1.setI2CAddress(0x7a);
  disp_2.setI2CAddress(0x78);

  u8g2.begin();
  

  delay(500);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // Linea muy importante en ESP32 para que el dispositivo no funciona con ahorro de energia, ya que si es asi... no funciona bien el WIFI. (microcortes)
  // esp_wifi_set_ps(WIFI_PS_NONE); // WiFi.setSleep(flase);   // Linea muy importante en ESP32 para que el dispositivo no funciona con ahorro de energia, ya que si es asi... no funciona bien el WIFI. (microcortes)
  ID = obtenerID();

  sprintf(TOKEN, "%i", ID);

  Serial.println();
  Serial.print("-----------   INICIO CELL SIGNAL TESTER = ");
  Serial.print(ID);
  Serial.println("    -----------");
  Serial.print("FW Ver: ");
  Serial.println(FW_VERSION);
  Serial.println();

  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_t0_11_mf);
    u8g2.setFontPosCenter();
    u8g2.drawStr(30, 32, "INICIANDO...");
  } while (u8g2.nextPage());

  digitalWrite(SIM_POWER, HIGH);

  // hardwareDetectAndConfigure();

  if (hardwareDetectado.SIM != 0)
  {
    Serial.println("GPS Activado");
    // modem.enableGPS();
  }

  Serial.println();
  Serial.println("PROGRAM RUNNING");

  tiempoDesconeccionCellNetwork = millis();
}

void loop()
{
  // st_datosGPS datosGPS;
  byte press = 0;

  while (1)
  {
    // obtenerFechaHoraCELL();  // Ojo cunciona solo cuando esta conectado a 4G, con gsm trae cualquier fecha
    mantieneConexionCELL();

    sensar();

    primerInicio = false;
  }

  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_t0_11_mf);
    u8g2.setFontPosCenter();
    u8g2.drawStr(10, 20, "ERROR CONEXION WIFI");
    u8g2.drawStr(10, 50, "REINICIANDO...");
  } while (u8g2.nextPage());

  delay(2000);
}
