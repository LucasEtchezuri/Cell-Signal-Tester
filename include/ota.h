
uint32_t knownFileSize = 1024;

void printPercent(uint32_t readLength, uint32_t contentLength, bool *i25, bool *i50, bool *i75)
{
    char topic[40];
    // char aux[500];
    //char msg[50];
    //sprintf(topic, "%s%d", MQTT_CH_UPDATE_STATUS, ID);

    // If we know the total length
    if (contentLength != (uint32_t)-1)
    {
        Serial.print("\r ");
        Serial.print(((readLength * 100.00) / contentLength) * 1.00);
        Serial.print('%');
    }
    else
    {
        Serial.println(readLength);
    }

    // float pos = ((readLength * 100.00 / contentLength) * 1.00);

    /* if (*i25 and pos > 25.00)
    {
        *i25 = false;
        sprintf(msg, "Updating firmware... 25%");
        mqttSIM.publish(topic, msg);
        mqttSIM.loop();
    }
    if (*i50 and pos > 50.00)
    {
        *i50 = false;
        sprintf(msg, "Updating firmware... 50%");
        mqttSIM.publish(topic, msg);
        mqttSIM.loop();
    }
    if (*i75 and pos > 75.00)
    {
        *i75 = false;
        sprintf(msg, "Updating firmware... 75%");
        // mqttSIM.publish(topic, msg);
        // mqttSIM.loop();
    } */
}

void otaUpdate(const char *server, uint16_t port, const char *resource)
{
    // #define TINY_GSM_RX_BUFFER 1024
    TinyGsmClient client(modem, 1);

    if (!client.connect(server, port))
    {
        Serial.println(" fail");
        client.stop();
        digitalWrite(SIM_POWER, LOW);
        delay(500);
        ESP.restart();
    }
    uint8_t buff[128] = {0};

    // Make a HTTP GET request:
    client.print(String("GET ") + resource + " HTTP/1.0\r\n");
    client.print(String("Host: ") + server + "\r\n");
    client.print("Connection: close\r\n\r\n");

    // Let's see what the entire elapsed time is, from after we send the request.
    uint32_t timeElapsed = millis();
    Serial.println(F("Waiting for response header"));
    // While we are still looking for the end of the header (i.e. empty line
    // FOLLOWED by a newline), continue to read data into the buffer, parsing each
    // line (data FOLLOWED by a newline). If it takes too long to get data from
    // the client, we need to exit.

    const uint32_t clientReadTimeout = 5000;
    uint32_t clientReadStartTime = millis();
    String headerBuffer;
    bool finishedHeader = false;
    uint32_t contentLength = 0;

    while (!finishedHeader)
    {
        esp_task_wdt_reset();
        int nlPos;
        if (client.available())
        {
            clientReadStartTime = millis();
            while (client.available())
            {
                esp_task_wdt_reset();
                char c = client.read();
                headerBuffer += c;

                // Uncomment the lines below to see the data coming into the buffer
                // if (c < 16)
                //    Serial.print('0');
                // Serial.print(c, HEX);
                // Serial.print(' ');
                // if (isprint(c))
                //    Serial.print(c);
                // else
                //    Serial.print('*');
                // Serial.print(' ');

                // Let's exit and process if we find a new line
                if (headerBuffer.indexOf(F("\r\n")) >= 0)
                    break;
            }
        }
        else
        {
            if (millis() - clientReadStartTime > clientReadTimeout)
            {
                // Time-out waiting for data from client
                Serial.println(F(">>> Client Timeout !"));
                break;
            }
        }

        // See if we have a new line.
        nlPos = headerBuffer.indexOf(F("\r\n"));

        if (nlPos > 0)
        {
            headerBuffer.toLowerCase();
            // Check if line contains content-length
            if (headerBuffer.startsWith(F("content-length:")))
            {
                contentLength =
                    headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
                Serial.print(F("Got Content Length: ")); // uncomment for
                Serial.println(contentLength);           // confirmation
            }

            headerBuffer.remove(0, nlPos + 2); // remove the line
        }
        else if (nlPos == 0)
        {
            // if the new line is empty (i.e. "\r\n" is at the beginning of the line),
            // we are done with the header.
            finishedHeader = true;
        }
    }

    uint32_t readLength = 0;
    // if (finishedHeader && contentLength == knownFileSize)

    char topic[40];
    char msg[50];
    //sprintf(topic, "%s%d", MQTT_CH_UPDATE_STATUS, ID);
    //int ultimoAviso = 0;
    bool upd25 = true;
    bool upd50 = true;
    bool upd75 = true;

    if (finishedHeader && contentLength > 0)
    {
        Serial.println(F("Reading response data"));
        Update.begin(UPDATE_SIZE_UNKNOWN);
        clientReadStartTime = millis();

        while (readLength < contentLength && client.connected() && millis() - clientReadStartTime < clientReadTimeout)
        {
            if (client.available())
            {
                uint8_t c = client.read();
                // SerialMon.print(reinterpret_cast<char>c);  // Uncomment this to show
                // data
                Update.write(&c, 1);
                readLength++;
                if (readLength % (contentLength / 104) == 0)
                {
                    esp_task_wdt_reset();
                    printPercent(readLength, contentLength, &upd25, &upd50, &upd75); // Da error el update si lo habilito.
                }
                clientReadStartTime = millis();
            }
        }
    }
    Update.end(true);
    Serial.println("Update Success.  Rebooting...");
    Serial.println();
    // Shutdown
    delay(500);
    client.stop();
    digitalWrite(SIM_POWER, LOW);
    delay(500);

    ESP.restart();
}

RPC_Response otaCELL(const RPC_Data &data)
{
    update = true;
    // Process data
    //const float example_temperature = data["ota"];
    
    strcpy(ota_url, data["url"]);
    strcpy(ota_file, data["file"]);
    ota_port = data["port"];
            
    // Just an response example
    StaticJsonDocument<JSON_OBJECT_SIZE(1)> doc;
    //doc["respuesta"] = 42;
    return RPC_Response(doc);
}
