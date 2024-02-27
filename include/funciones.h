

float voltajeBateria()
{
    uint16_t val = 0;

    for (byte i = 1; i < 5; i++)
    {
        val = (val + analogRead(PIN_SENSOR_VBAT)) / 2;
        delay(25);
    }
    val = (map(val, 1983, 2914, 600, 840) + CORRECCION_VOLT_BATERIA);
    val = constrain(val, 0, 1000);
    return (val / 100.00);
}

void hardwareDetectAndConfigure()
{
    pinMode(PIN_SENSOR_VBAT, INPUT);

    // Detect SIM
    hardwareDetectado.SIM = 0;

    SerialSIM.begin(115200, SERIAL_8N1, PIN_SIM_RX, PIN_SIM_TX, false);
    delay(150);

    Serial.println("Esperando inicio modulo SIM (25 seg):");
    for (byte x = 0; x < 20; x++) // Espero a que incie el modulo SIM.  Ojo que si no espero lo sificiente, no funciona
    {
        Serial.print(">");
        if (modem.testAT(1000))
        {
            x = 50;
        }
        else
        {
            delay(1000);
        }
    }

    delay(100);
    Serial.println("");

    Serial.print("SIM STATUS = ");

    byte simStatus = 0;

    simStatus = modem.getSimStatus();

    if (simStatus == 0)
    {
        Serial.println("SIM_ERROR");
        modem.restart();
        delay(1000);
        ESP.restart();
    }
    else if (simStatus == 1)
        Serial.println("SIM_READY");
    else if (simStatus == 2)
    {
        Serial.println("SIM_LOCKED");
        modem.restart();
        delay(1000);
        ESP.restart();
    }
    else if (simStatus == 3)
    {
        Serial.println("SIM_ANTITHEFT_LOCKED");
        modem.restart();
        delay(1000);
        ESP.restart();
    }
    else
        Serial.println("DESCONOCIDO");

    delay(100);

    modemName = modem.getModemName();
    modemName.replace("\r", "");
    modemName.replace("\n", "");
    modemName.trim();

    delay(100);

    if (modemName.indexOf("SIM7000") != -1)
    {
        Serial.print("SIM Detectado = 2-Modulo 7000G IoT (");
        Serial.print(modemName);
        Serial.println(")");
        hardwareDetectado.SIM = 2;
    }
    if (modemName.indexOf("SIM7600") != -1)
    {
        Serial.println("SIM Detectado = 4-Modulo 7600G 4G (");
        Serial.print(modemName);
        Serial.println(")");
        hardwareDetectado.SIM = 4;
    }

    // 0 = Not detected.  2=SIM7000G Module    4=SIM7600G

    if (hardwareDetectado.SIM == 2)
    {
#ifdef TINY_GSM_MODEM_SIM7000
        // modem.setNetworkMode(51); // 2=automatic   13=gsm only   38=lte only   51=gsm y lte
        // if (modem.getNetworkMode() != 38)

        if (true)
        {
            modem.setNetworkMode(38);
            Serial.println("Se setea modulo en modo red 38 (LTE CatM)");
            delay(50);
            modem.setPreferredMode(1); // Cat-M
            // modem.setPreferredMode(2); // NB
            // modem.setPreferredMode(3); // Cat-M and NB
        }
        else
        {
            modem.setNetworkMode(13);
            Serial.println("Se setea modulo en modo red 13 (gsm)");
        }

        delay(50);
        Serial.print("Buscando Red Celular ");

        for (byte x = 0; x < 10; x++)
        {
            if (modem.waitForNetwork(5000))
            {
                Serial.println("OK");
                x = 20;
            }
            else
                Serial.print(".");
        }

#else
        Serial.println("El programa no esta configurado para el modulo detectado.");
        Serial.println("Programa configurado para modulo: SIM7000G");
        Serial.print("Modulo detectado: ");
        Serial.println(modemName);
        delay(500);
        ESP.restart();
#endif
    }
    else if (hardwareDetectado.SIM == 4)
    {
#ifdef TINY_GSM_MODEM_SIM7600
        // modem.setNetworkMode(51); // 2=automatic   13=gsm only   38=lte only   51=gsm y lte 67 =CDMA+EVDO+GSM+WCDMA+TDSCDMA Only

        modem.setNetworkMode(38);
        Serial.println("Se setea modulo en modo red 38 (LTE CatM)");

        delay(50);

        Serial.print("Buscando Red Celular ");

        for (byte x = 0; x < 10; x++)
        {
            if (modem.waitForNetwork(5000))
            {
                Serial.println("OK");
                x = 20;
            }
            else
                Serial.print(".");
        }
#else
        Serial.println("El programa no esta configurado para el modulo detectado.");
        Serial.println("Programa configurado para modulo: SIM7600G");
        Serial.print("Modulo detectado: ");
        Serial.println(modemInfo);
        delay(500);
        ESP.restart();
#endif
    }
    else
    {
        Serial.println("No se detecto modulo SIM - REINICIANDO (SIM Y MODULO)");
        delay(50);
        digitalWrite(SIM_POWER, LOW);
        // modem.restart();
        delay(1000);
        ESP.restart();
    }
}

bool obtenerDatosGPS(st_datosGPS *gps)
{
    bool res = false;
    res = modem.getGPS(&gps->latitud, &gps->longitud, &gps->velocidad, &gps->altitud, &gps->vsat, &gps->usat, &gps->precision, &gps->anio, &gps->mes, &gps->dia, &gps->hora, &gps->minuto, &gps->segundo);
    return res;
}

void sensarGPS()
{

    // obtenerDatosGPS();
}

void sensar()
{
    if (hardwareDetectado.SIM != 0)
    {
        sensarGPS();
    }
}

bool SimConnectGprs()
{
    if (modem.isNetworkConnected())
    {
        String CellOperator = "";
        CellOperator = modem.getOperator();

        if (CellOperator.indexOf("Movistar") != -1)
        {
            Serial.print("Connecting to Movistar: ");
            Serial.println(CELL_MOVISTAR_APN);
            if (!modem.gprsConnect(CELL_MOVISTAR_APN, CELL_MOVISTAR_USER, CELL_MOVISTAR_PASS))
                return false;
            return true;
        }
        else if (CellOperator.indexOf("Claro") != -1)
        {
            Serial.print("Connecting to Claro: ");
            Serial.println(CELL_CLARO_APN);
            if (!modem.gprsConnect(CELL_CLARO_APN, CELL_CLARO_USER, CELL_CLARO_PASS))
                return false;
            return true;
        }
        else if (CellOperator.indexOf("Personal") != -1)
        {
            Serial.print("Connecting to Personal: ");
            Serial.println(CELL_PERSONAL_APN);
            if (!modem.gprsConnect(CELL_PERSONAL_APN, CELL_PERSONAL_USER, CELL_PERSONAL_PASS))
                return false;
            return true;
        }
        else
        {
            Serial.print("Operador detectado no configurado: ");
            Serial.println(CellOperator);
            return false;
        }
    }
    return false;
}

bool SimNetworkConnect()
{
    modem.setNetworkMode(38);

#ifdef TINY_GSM_MODEM_SIM7000
    modem.setPreferredMode(1);
#endif

    Serial.print("Buscando Red Celular 4G");
    for (byte x = 0; x < 4; x++)
    {
        if (modem.waitForNetwork(5000))
        {
            Serial.println("OK");
            Serial.print("Conectado a la red del operador = ");
            Serial.println(modem.getOperator());
            x = 20;
            return true;
        }
        else
            Serial.print(".");
    }
    Serial.println("No se encontro redes 4G");
    Serial.println();

#ifdef TINY_GSM_MODEM_SIM7600
    modem.setNetworkMode(67);
    Serial.print("Buscando Red Celular 3G");
    for (byte x = 0; x < 4; x++)
    {
        if (modem.waitForNetwork(5000))
        {
            Serial.println("OK");
            Serial.print("Conectado a la red del operador = ");
            Serial.println(modem.getOperator());
            x = 20;
            return true;
        }
        else
            Serial.print(".");
    }

    Serial.println("No se encontro redes 3G");
    Serial.println();
#endif

    modem.setNetworkMode(13);
    Serial.print("Buscando Red Celular 2G");
    for (byte x = 0; x < 4; x++)
    {
        if (modem.waitForNetwork(5000))
        {
            Serial.println("OK");
            Serial.print("Conectado a la red del operador = ");
            Serial.println(modem.getOperator());
            x = 20;
            return true;
        }
        else
            Serial.print(".");
    }

    Serial.println("No se encontro red: Reiniciando Modulo");
    digitalWrite(SIM_POWER, LOW);
    delay(3000);
    ESP.restart();
    delay(1000);
    return false;
}

uint16_t obtenerID()
{
    ID = 0;

    uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
    uint16_t ID = (uint16_t)(chipid >> 32);
    return ID;
}

void mantieneConexionCELL()
{
    //   ------------------------   PRINCIPAL CELL   -----------------------------------------------

    if (hardwareDetectado.SIM != 0)
    {
        if (lastReconnectAttemptCell == 0 or millis() - lastReconnectAttemptCell > (configuracion.periodoCheckReintentoConexionCELL * 1000))
        {
            if (!modem.isNetworkConnected())
            {
                SimNetworkConnect(); // Solo se ejecuta si "configuracion.usarCell == true"
            }
            else
            {
                tiempoDesconeccionCellNetwork = millis();
            }

            if (!modem.isGprsConnected())
            {
                if (SimConnectGprs() == false)
                {
                    Serial.println("No se pudo conectar al APN: Reiniciando Modulo");
                    digitalWrite(SIM_POWER, LOW);
                    delay(3000);
                    ESP.restart();
                    delay(1000);
                };
            }
            lastReconnectAttemptCell = millis();
        }
    }
}
