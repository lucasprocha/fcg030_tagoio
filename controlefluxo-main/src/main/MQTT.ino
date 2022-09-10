////////////////// privates definitions
#define TAG "MQTT"

////////////////// privates functions
void onMqttConnect() {
  String lati = readFile(CONFIG_FOLDER CONFIG_LAT);
  String longi = readFile(CONFIG_FOLDER CONFIG_LONG);

  if (lati != "" && longi != "") {
    DLOG(TAG, "Sending the location\r\n");
    DynamicJsonDocument doc(256);
    doc["variable"] = "location";
    doc["value"] = "My Address";
    doc["location"]["lat"] = lati.toDouble();
    doc["location"]["lng"] = longi.toDouble();

    String output;
    serializeJson(doc, output);

    mqttClient.publish("location", output.c_str(), true);
  }

  DLOG(TAG, "Sending the IP\r\n");
  DynamicJsonDocument doc(256);
  doc["variable"] = "ip";
  doc["value"] = WiFi.localIP().toString();

  String output;
  serializeJson(doc, output);

  mqttClient.publish("ip", output.c_str(), true);

  DLOG(TAG, "Subscribing the outputs\r\n");
  mqttClient.subscribe("outputs");
}

void outputsHandler(String payload) {
  String str = payload;
  str.replace("output", "");
  uint8_t equalIndex = str.indexOf("=");
  String pin = str.substring(0, equalIndex);
  uint8_t pin_n = pin.toInt();
  String mode = str.substring(equalIndex + 1);
  uint8_t mode_n = mode == "true" ? 1 : 0;
  DLOG("OUTPUTS", "PIN: %s - MODE: %s\r\n", pin.c_str(), mode.c_str());

  if (pin == "disp_esp") {
    digitalWrite(DISP_ESP, mode_n);
  } else if (pin == "buzzer_esp") {
    digitalWrite(BUZZER_ESP, mode_n);
  } else if (pin == ("com_product_esp")) {
    digitalWrite(COM_PRODUCT_ESP, mode_n);
  } else {
    String status = readFile(OUTPUT_PIN_STATUS);
    DynamicJsonDocument doc(512);
    deserializeJson(doc, status);
    doc[pin_n] = mode_n;
    String output;
    serializeJson(doc, output);
    writeFile(OUTPUT_PIN_STATUS, output.c_str());
    send_to_mpc_i2c();
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr;
  for (int i = 0; i < length; i++) {
    payloadStr += String((char)payload[i]);
  }
  ILOG(TAG, "Message from broker\r\n\tTopic: %s\r\n\tPayload: %s\r\n", topic, payloadStr.c_str());

  if (!strcmp(topic, "outputs")) {
    outputsHandler(payloadStr);
  } 
}

////////////////// publics functions
void send_inputs_to_broker(const char *topic, double inputs) {
  if (WiFi.status() != WL_CONNECTED) return;
  String str = String(topic);
  int index = str.lastIndexOf('/');
  if (index != -1) {
    str = str.substring(index + 1);
  }
  DynamicJsonDocument doc(256);
  doc["variable"] = str.c_str();
  doc["value"] = inputs;

  String output;
  serializeJson(doc, output);
  DLOG("SEND", "Sending to topic '%s':\r\n%s\r\n", topic, output.c_str());
  mqttClient.publish(topic, output.c_str(), true);
}

void send_inputs_to_broker(const char *topic, int inputs) {
  if (WiFi.status() != WL_CONNECTED) return;
  String str = String(topic);
  int index = str.lastIndexOf('/');
  if (index != -1) {
    str = str.substring(index + 1);
  }
  DynamicJsonDocument doc(256);
  doc["variable"] = str.c_str();
  doc["value"] = inputs;

  String output;
  serializeJson(doc, output);
  DLOG("SEND", "Sending to topic '%s':\r\n%s\r\n", topic, output.c_str());
  mqttClient.publish(topic, output.c_str(), true);
}

boolean reconnect_mqtt() {
  WLOG(TAG, "Trying to reconnect on TAGO.io\r\n");
  String broker = readFile(CONFIG_FOLDER CONFIG_BROKER);
  String token = readFile(CONFIG_FOLDER CONFIG_TOKEN);
  String port = readFile(CONFIG_FOLDER CONFIG_PORT);

  if (token == "") {
    WLOG(TAG, "Device has not a token\r\n");
    return -1;
  }

  DLOG(TAG, "MQTT Host:\t %s\r\n", broker.c_str());
  DLOG(TAG, "MQTT Port:\t %i\r\n", port.toInt());
  DLOG(TAG, "MQTT Username:\t %s\r\n", MQTT_USERNAME);
  DLOG(TAG, "MQTT token:\t %s\r\n", token.c_str());

  if (broker == "") {
    WLOG(TAG, "Device has not a broker server\r\n");
    return -1;
  }

  DLOG(TAG, "Connecting to the MQTT server\r\n");
  mqttClient.setServer(broker.c_str(), port.toInt());
  mqttClient.setCallback(callback);
  if (mqttClient.connect("esp32", MQTT_USERNAME, token.c_str())) {
    ILOG(TAG, "Reconnection done\r\n");
    onMqttConnect();
  }
  return mqttClient.connected();
}


int init_mqtt() {
  DLOG(TAG, "Initialize the MQTT\r\n");
  String broker = readFile(CONFIG_FOLDER CONFIG_BROKER);
  String token = readFile(CONFIG_FOLDER CONFIG_TOKEN);
  String port = readFile(CONFIG_FOLDER CONFIG_PORT);

  DLOG(TAG, "MQTT Host:\t %s\r\n", broker.c_str());
  DLOG(TAG, "MQTT Port:\t %i\r\n", port.toInt());
  DLOG(TAG, "MQTT Username:\t %s\r\n", MQTT_USERNAME);
  DLOG(TAG, "MQTT token:\t %s\r\n", token.c_str());

  if (token == "") {
    WLOG(TAG, "Device has not a token\r\n");
    return -1;
  }

  DLOG(TAG, "Connecting to the MQTT server\r\n");
  mqttClient.setServer(broker.c_str(), port.toInt());
  mqttClient.setCallback(callback);
  if (!mqttClient.connect("esp32", MQTT_USERNAME, token.c_str())) {
    WLOG(TAG, "Could not connect on brokerr\n");
    return -1;
  }
  ILOG(TAG, "Connected to MQTT server\r\n");

  onMqttConnect();

  return 0;
}
