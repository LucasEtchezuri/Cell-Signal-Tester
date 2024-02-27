// ------- Datos GPS
struct st_datosGPS
{
    long timestamp;
    float latitud = 0.0;
    float longitud = 0;
    float velocidad = 0;
    float altitud = 0;
    int vsat = 0;
    int usat = 0;
    float precision = 0;
    int anio = 0;
    int mes = 0;
    int dia = 0;
    int hora = 0;
    int minuto = 0;
    int segundo = 0;
    st_datosGPS *ptr_next;
};

// ------- hardware detectado
struct st_hardwareDetectado
{
    int SIM = 0; // 0 = Not detected.    1=SIM7000G     2=SIM7000G Module     3=SIM800L
    bool SD = false;
    int8_t SD_TYPE = 0;
    uint64_t SD_SIZE_MB = 0; // 0=No detected,   1=MMC,   2=SDSC,   3=SDHC,    -1=UNKNOW
};

// ------- Status
struct st_status
{
    uint8_t versionConfiguracion = 0;
    long ultimoReinicio = 0;
    byte motivoReinicio = 0;

    /*
    1 = POWERON_RESET
    3 = SW_RESET
    4 = OWDT_RESET
    5 = DEEPSLEEP_RESET
    6 = SDIO_RESET
    7 = TG0WDT_SYS_RESET
    8 = TG1WDT_SYS_RESET
    9 = RTCWDT_SYS_RESET
    10 = INTRUSION_RESET
    11 = TGWDT_CPU_RESET
    12 = SW_CPU_RESET
    13 = RTCWDT_CPU_RESET
    14 = EXT_CPU_RESET
    15 = RTCWDT_BROWN_OUT_RESET
    16 = RTCWDT_RTC_RESET
    */
};
// ------- Configuracion
struct st_configuracion
{
    uint8_t version = 0;
    uint8_t periodoSensadoGPS = 90;
    //uint8_t periodoActualizarStatus = 30;
    uint8_t periodoObtenerFechaCELL = 30;           // Es el periodo que intenta obtener la fecha hora si es que nunca la obtuvo.
    uint16_t periodoActualizarFechaCELL = 300;      // Si ya tiene fecha y hora es el tiempo en que lo va a intentar resincronizar
    uint8_t periodoCheckReintentoConexionCELL = 30; //  Verifica cada X segundos si esta conextado CELL y reintenta.
    uint8_t periodoCheckReintentoConexionMQTT = 60; // Verifica conexion y reintenta. (para todo tipo de conexiones).
    u_int16_t periodoTransmisionTelemetria = 60;
    u_int16_t periodoTransmisionTelemetriaGPS = 60;
    u_int16_t periodoTransmisionHubStatus = 300;
    bool usarGPS = true;
    uint8_t tiempoMaximoDesconectadoCell = 120;
    uint8_t periodoCheckRecoveryTransmision = 30;
    uint32_t aux1 = 0; // no lo uso, pero si esta estructura tiene menos tamaño da error.
    uint32_t aux2 = 0; // no lo uso, pero si esta estructura tiene menos tamaño da error.
    uint32_t aux3 = 0; // no lo uso, pero si esta estructura tiene menos tamaño da error.

    uint32_t crc = 0;
};

// ------- 18B20
struct st_SHT
{
    long timestamp;
    float temp;
    float hum;
    st_SHT *ptr_next;
};

struct st_INA01
{
    long timestamp;
    int32_t id;
    uint8_t nroEnvio;
    char modelo[20];
    float FW_Version;
    unsigned int sensInterval;
    float temp;
    float hum;
    float bat;
    st_INA01 *ptr_next;
};

// ------- Mediciones
struct st_mediciones
{
    long timestamp;
    st_INA01 *ptr_INA01 = NULL;
    st_datosGPS *ptr_GPS = NULL;
};

struct st_dato_v1
{
    int16_t datoValido; // 100 = Dato valido
    int16_t FW_Version;
    uint32_t configVersion; // Version de la configuracion del ES
    int16_t timeInterval;   // Intervalo de tiempo configurado
    float temperature;
    float battery;
};

struct st_dato_v2
{
    uint8_t nroEnvio;
    char modelo[20];
    float FW_Version;
    unsigned int sensInterval; // Intervalo de tiempo configurado
    float temperature;
    float humidity;
    float battery;
};

struct st_dato_v3
{
    uint32_t secuencia;
    uint8_t nroEnvio;
    char modelo[20];
    float FW_Version;
    unsigned int sensInterval; // Intervalo de tiempo configurado
    float temperature;
    float humidity;
    float battery;
    float temperatureAnt;
    float humidityAnt;
};

struct st_inalambricos
{
    int32_t mac;
    unsigned long time = 0;
};

struct st_INA_Repetidos
{ // estructuras para guardar los paquetes transmitidos y descartar los repetidos. (el INA envia 2 paquetes seguidos para aumentar la probabilidad que lleguen)
    int32_t mac;
    unsigned long milisegundos;
};

struct st_INA_UltimaSecuenciaRecibida
{ // estructuras para guardar el ultimo nro de secuancia recibido de cada HUB.  Se utiliza para ver si hace falta enviar los datos Ateriores, que son recibidos desde la v3 de la trama.
    int32_t mac;
    unsigned long milisegundos;
    uint32_t secuencia;
};



