////////////////// privates definitions
#define TAG "RFID"

uint16_t machineTime = MACHINE_TIMEOUT_S;

////////////////// privates functions
void disableMachine(){
  ILOG(TAG, "Disable the machine\r\n");
  digitalWrite(DISP_ESP, HIGH);
  //digitalWrite(COM_PRODUCT_ESP, HIGH);//lucas
}

void IRAM_ATTR onTimer(){
  machineTime--;
  if (machineTime == 0) {
    timerEnd(timer);
    timer = NULL;
    disableMachine();
    
  }
}

////////////////// publics functions
void getTagsList() {
  DLOG(TAG, "Getting the tags list\r\n");
  String url = readFile(CONFIG_FOLDER CONFIG_TAGS_URL);
  if (url == "") {
    WLOG(TAG, "Do not find the url for get the tags list\r\n");
    return;
  };
  int ret = 0;
  client.begin(url.c_str());
  int resp = client.GET();
  DLOG(TAG, "Response: %i\r\n", resp);
  if (resp == 302) {
    String respStr = client.getString();
    int first = respStr.indexOf("http");
    client.end();
    if (first == -1) {
      return;
    }
    client.begin((respStr.substring(first)).c_str());
    resp = client.GET();
  }
  if (resp == 200) {
    totalLength = client.getSize();
    float len = totalLength;
    currentLength = 0;
    uint8_t buff[128] = { 0 };
    WiFiClient * stream = client.getStreamPtr();
    DLOG(TAG, "File Size: %.0f\r\n", totalLength);
    writeFile(TAGS_FILE, "");
    while (client.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        String str;
        for (int i = 0; i < c; i++) str += (char)buff[i];
        appendFile(TAGS_FILE, str.c_str(), false);
        currentLength += c;
        counter++;
        if (counter == 50) {
          counter = 0;
          DLOG(TAG, "%.0f/%.0f\r\n", currentLength, totalLength);
        }
        if (currentLength == totalLength) break;
        if (len > 0) {
          len -= c;
        }
      }
      delay(2);
    }
  }
  client.end();
}

void readRfid() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) 
    return; 

  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX);
  }
  strID.toUpperCase();

  ILOG(TAG, "TAG id: %s\r\n", strID.c_str());

  String tagsList = readFile(TAGS_FILE);
  if (tagsList.indexOf(strID) != -1){
    ILOG(TAG, "TAG ok, turn on the machine\r\n");
    
    timer = timerBegin(0, 80, true);
    if (timer == NULL) {
      ELOG(TAG, "Fail to start the timer\r\n");
      return;
    }
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000000, true);// 1000000 - teste para rfid mais rapido
    timerAlarmEnable(timer);
    machineTime = MACHINE_TIMEOUT_S;
    //com_esp=digitalRead(COM_PRODUCT_ESP);//lucas
    // Serial.print(com_esp);//lucas
    digitalWrite(DISP_ESP, LOW); 
    
    digitalWrite(BUZZER_ESP, HIGH);  // lucas beep para positivo também
    delay(200);
    digitalWrite(BUZZER_ESP, LOW);
    //digitalWrite(COM_PRODUCT_ESP, HIGH);//lucas
    
  } else {
    ELOG(TAG, "TAG not allowed\r\n");
    digitalWrite(BUZZER_ESP, HIGH);
    delay(200);
    digitalWrite(BUZZER_ESP, LOW);
    delay(200);
    digitalWrite(BUZZER_ESP, HIGH);
    delay(200);
    digitalWrite(BUZZER_ESP, LOW);
    digitalWrite(COM_PRODUCT_ESP, HIGH);//lucas
  }

  rfid.PICC_HaltA(); 
  rfid.PCD_StopCrypto1(); 
}
