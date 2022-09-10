////////////////// privates definitions
#define TAG "WIFI"

////////////////// privates functions
void serverHandler() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(DEFAULT_USERNAME, DEFAULT_USERPASSWORD))
      return request->requestAuthentication();
    request->send(SPIFFS, WEBPAGE_FOLDER WEBPAGE_HTML_FILE, "text/html");
  });

  server.on("/static/js/"WEBPAGE_FILE_1, HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, WEBPAGE_FOLDER WEBPAGE_FILE_1, "text/js");
  });

  server.on("/static/js/"WEBPAGE_FILE_2, HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, WEBPAGE_FOLDER WEBPAGE_FILE_2, "text/js");
  });

  server.on("/static/js/"WEBPAGE_FILE_3, HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, WEBPAGE_FOLDER WEBPAGE_FILE_3, "text/js");
  });

  server.on("/static/css/"WEBPAGE_FILE_4, HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, WEBPAGE_FOLDER WEBPAGE_FILE_4, "text/css");
  });

  server.on("/config/config", HTTP_GET, [](AsyncWebServerRequest * request) {
    String str = "{\"lat\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_LAT)) {
      String data = readFile(CONFIG_FOLDER CONFIG_LAT);
      str += data;
    }
    str += "\",\"long\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_LONG)) {
      String data = readFile(CONFIG_FOLDER CONFIG_LONG);
      str += data;
    }
    str += "\",\"broker\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_BROKER)) {
      String data = readFile(CONFIG_FOLDER CONFIG_BROKER);
      str += data;
    }
    str += "\",\"port\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_PORT)) {
      String data = readFile(CONFIG_FOLDER CONFIG_PORT);
      str += data;
    }
    str += "\",\"token\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_TOKEN)) {
      String data = readFile(CONFIG_FOLDER CONFIG_TOKEN);
      str += data;
    }
    str += "\",\"tags\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_TAGS_URL)) {
      String data = readFile(CONFIG_FOLDER CONFIG_TAGS_URL);
      str += data;
    }
    str += "\",\"calibration\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_CALIBRATION)) {
      String data = readFile(CONFIG_FOLDER CONFIG_CALIBRATION);
      str += data;
    }
    str += "\",\"calibration2\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_CALIBRATION2)) {
      String data = readFile(CONFIG_FOLDER CONFIG_CALIBRATION2);
      str += data;
    }
    str += "\",\"calibrationAna4\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_CALIBRATION_ANA_4)) {
      String data = readFile(CONFIG_FOLDER CONFIG_CALIBRATION_ANA_4);
      str += data;
    }
    str += "\",\"calibrationAna5\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_CALIBRATION_ANA_5)) {
      String data = readFile(CONFIG_FOLDER CONFIG_CALIBRATION_ANA_5);
      str += data;
    }
    str += "\",\"useRfid\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_USE_RFID)) {
      String data = readFile(CONFIG_FOLDER CONFIG_USE_RFID);
      str += data;
    }
    str += "\",\"flow1\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_FLOW_1)) {
      String data = readFile(CONFIG_FOLDER CONFIG_FLOW_1);
      str += data;
    }
    str += "\",\"flow2\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_FLOW_2)) {
      String data = readFile(CONFIG_FOLDER CONFIG_FLOW_2);
      str += data;
    }
    str += "\",\"serial\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_SERIAL)) {
      String data = readFile(CONFIG_FOLDER CONFIG_SERIAL);
      str += data;
    }
    str += "\",\"model\":\"";
    if (existFile(CONFIG_FOLDER CONFIG_MODEL)) {
      String data = readFile(CONFIG_FOLDER CONFIG_MODEL);
      str += data;
    }
    str += "\"}";
    DLOG("CONFIG", "str: %s\r\n", str.c_str());
    request->send(200, "application/json", str);
  });

  server.on("/config/config", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->hasParam("body", true)) {
      AsyncWebParameter* p = request->getParam("body", true);
      DLOG("SERVER", "Configurações recebidas em /config/config\r\n");
      DLOG("SERVER", "%s\r\n", String(p->value()).c_str());
      String payload = String(p->value());
      DynamicJsonDocument doc(payload.length() + 128);
      deserializeJson(doc, payload);
      String lati = doc["lat"];
      writeFile(CONFIG_FOLDER CONFIG_LAT, lati.c_str());
      String longi = doc["long"];
      writeFile(CONFIG_FOLDER CONFIG_LONG, longi.c_str());
      String broker = doc["broker"];
      writeFile(CONFIG_FOLDER CONFIG_BROKER, broker.c_str());
      String port = doc["port"];
      writeFile(CONFIG_FOLDER CONFIG_PORT, port.c_str());
      String tags = doc["tags"];
      writeFile(CONFIG_FOLDER CONFIG_TAGS_URL, tags.c_str());
      String calibration = doc["calibration"];
      writeFile(CONFIG_FOLDER CONFIG_CALIBRATION, calibration.c_str());
      String calibration2 = doc["calibration2"];
      writeFile(CONFIG_FOLDER CONFIG_CALIBRATION2, calibration2.c_str());
      String token = doc["token"];
      writeFile(CONFIG_FOLDER CONFIG_TOKEN, token.c_str());
      String useRfid = doc["useRfid"];
      writeFile(CONFIG_FOLDER CONFIG_USE_RFID, useRfid.c_str());
      String flow1 = doc["flow1"];
      writeFile(CONFIG_FOLDER CONFIG_FLOW_1, flow1.c_str());
      String flow2 = doc["flow2"];
      writeFile(CONFIG_FOLDER CONFIG_FLOW_2, flow2.c_str());
      String serial = doc["serial"];
      writeFile(CONFIG_FOLDER CONFIG_SERIAL, serial.c_str());
      String model = doc["model"];
      writeFile(CONFIG_FOLDER CONFIG_MODEL, model.c_str());
      request->send(200);
      ILOG(TAG, "Restarting the system for apply the configs.\r\n");
      ESP.restart();
    }
    request->send(400);
  });

  server.on("/ip", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", "{\"localIp\":\"" + WiFi.localIP().toString() + "\"}");
  });

  server.on("/flow", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", "{\"flow1\":" + String(flow1) + ", \"flow2\":\"" + String(flow2) +  "\"}");
  });

  server.on("/inputs", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", "{\"input\":" + String(input) + ",\"ANA_IN_4\":" + String(analog_inputs[0]) + ",\"ANA_IN_5\":" + String(analog_inputs[1]) + "}");
  });

  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(401);
  });

  server.begin();
}

String getSsidByIndex(uint8_t index) {
  return readFile(index == 0 ? SSID_FILE : (String(SSID_FILE) + String(index)).c_str());
}

String getPassByIndex(uint8_t index) {
  return readFile(index == 0 ? PASS_FILE : (String(PASS_FILE) + String(index)).c_str());
}

uint8_t getMaxIndex() {
  uint8_t counter = 0;
  while (existFile(counter == 0 ? SSID_FILE : (String(SSID_FILE) + String(counter)).c_str())) counter++;
  return counter;
}

bool verifyNetwork(String *referSSID, String *referPASS) {
  int n = WiFi.scanNetworks();
  if (n == 0) return false;
  uint8_t maxIndex = getMaxIndex();
  if (maxIndex == 0) return false;
  int rssi[64][2];
  int counter = 0;
  bool findOne = false;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < maxIndex; j++) {
      DLOG(TAG, "WIFI SCAN: %s -> Wifi SAVED: %s\r\n", String(WiFi.SSID(i)).c_str(), String(getSsidByIndex(j)).c_str());
      if (String(WiFi.SSID(i)) == getSsidByIndex(j)) {
        rssi[counter][0] = j;
        rssi[counter][1] = WiFi.RSSI(i);
        findOne = true;
      }
    }
  }
  if (!findOne) return false;
  uint8_t lowRssi[2];
  lowRssi[0] = rssi[0][0];
  lowRssi[1] = rssi[0][1];
  for (int i = 1; i < counter; i++) {
    if (rssi[i][1] > lowRssi[1]) {
      lowRssi[0] = rssi[i][0];
      lowRssi[1] = rssi[i][1];
    }
  }
  *referSSID = readFile((lowRssi[0] == 0 ? String(SSID_FILE) : String(SSID_FILE) + String(lowRssi[0])).c_str());
  *referPASS = readFile((lowRssi[0] == 0 ? String(PASS_FILE) : String(PASS_FILE) + String(lowRssi[0])).c_str());
  return true;
}

////////////////// publics functions
void init_wifi()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  String ssid, pass;
  if (verifyNetwork(&ssid, &pass)) {
    ILOG(TAG, "Connecting to Wifi... %s\r\n", ssid.c_str());
    if (strlen(pass.c_str()) > 0) {
      WiFi.begin(ssid.c_str(), pass.c_str());
    } else {
      WiFi.begin(ssid.c_str());
    }
    //while (WiFi.status() != WL_CONNECTED) delay(1000);
    if (WiFi.status() != WL_CONNECTED)  // lucas
    {
      uint8_t aux2 = 0;
      for (; aux2 < 25; aux2++)
      {
        delay(1000);
        // ILOG(TAG, "Tentando wifi... %s\r\n", aux2);
        if (WiFi.status() == WL_CONNECTED) aux2 = 30; //
      }
      if (aux2 != 31) {
        WLOG(TAG, "Iniciando sem WiFi\r\n");
        return;
      }
    }  // lucas
  } else {
    WiFi.beginSmartConfig();
    isSmartConfigOn = true;
    ILOG(TAG, "Waiting for SmartConfig.\r\n");
    smartConfigCheck.attach(1, wifi_verify_smartconfig);
    return;
  }

  ILOG(TAG, "WiFi Connected\r\n");
  DLOG(TAG, "IP Address: %s\r\n", WiFi.localIP().toString().c_str());

  serverHandler();
  ILOG("SERVER", "Servidor rodando\r\n");
}

void wifi_reconnect()
{
  if (WiFi.status() != WL_CONNECTED && !isSmartConfigOn) {
    init_wifi();
  }
}

void wifi_verify_smartconfig_connection() {
  ILOG(TAG, "wifi_verify_smartconfig_connection %i\r\n", connect_try);
  if (WiFi.status() != WL_CONNECTED)  // lucas
  {
    connect_try++;
    if (connect_try > 30) {
      WLOG(TAG, "Iniciando sem WiFi\r\n");
      smartConfigConnectCheck.detach();
      connect_try = 0;
    }
    return;
  }
  connect_try = 0;  
  WiFi.stopSmartConfig();
  isSmartConfigOn = false;
  uint8_t maxIndex = getMaxIndex();
  writeFile((String(SSID_FILE) + (maxIndex == 0 ? "" : String(maxIndex))).c_str(), WiFi.SSID().c_str());
  if (strlen(WiFi.psk().c_str()) > 0) {
    writeFile((String(PASS_FILE) + (maxIndex == 0 ? "" : String(maxIndex))).c_str(), WiFi.psk().c_str());
  }
  ILOG(TAG, "WiFi Connected\r\n");
  DLOG(TAG, "IP Address: %s\r\n", WiFi.localIP().toString().c_str());

  serverHandler();
  ILOG("SERVER", "Servidor rodando\r\n");
  
  smartConfigConnectCheck.detach();
}

void wifi_verify_smartconfig() {
  if (!WiFi.smartConfigDone()) {
    return;
  }
  ILOG(TAG, "SmartConfig received. Connecting to Wifi...\r\n");

  smartConfigConnectCheck.attach(1, wifi_verify_smartconfig_connection);
  smartConfigCheck.detach();
}
