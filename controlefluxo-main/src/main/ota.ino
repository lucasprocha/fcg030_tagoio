
#include "defines.h"
////////////////// privates definitions
#define TAG "OTA"

uint8_t counter = 0;

////////////////// privates functions
void updateFirmware(uint8_t *data, size_t len, String newVersion) {
  Update.write(data, len);
  currentLength += len;
  counter++;
  if (counter == 50) {
    counter = 0;
    DLOG(TAG, "%.0f/%.0f\r\n", currentLength, totalLength);
  }
  if (currentLength != totalLength) return;
  DLOG(TAG, "updated.end\r\n");
  Update.end(true);
  ILOG(TAG, "\n\rUpdate Success, Total Size: %.0f\n\rRebooting...\n\r", currentLength);
  writeFile(FW_FILE, newVersion.c_str());
  ESP.restart();
}

int otaUpdate(String url, String newVersion, int type = 0, bool fw = true) {
  int ret = 0;
  client.begin(url.c_str());
  int resp = client.GET();
  DLOG(TAG, "Response: %i\r\n", resp);
  if (resp == 302) {
    String respStr = client.getString();
    int first = respStr.indexOf("http");
    if (first == -1) return -1;
    client.end();
    client.begin((respStr.substring(first)).c_str());
    resp = client.GET();
  }
  if (resp == 200 && fw) {
    totalLength = client.getSize();
    float len = totalLength;
    Update.begin(UPDATE_SIZE_UNKNOWN);
    DLOG(TAG, "FW Size: %.0f\r\n", totalLength);
    uint8_t buff[128] = { 0 };
    WiFiClient * stream = client.getStreamPtr();
    ILOG(TAG, "Updating firmware...\r\n");
    while (client.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        updateFirmware(buff, c, newVersion);
        if (len > 0) {
          len -= c;
        }
      }
      delay(2);
    }
    WLOG(TAG, "Client disconnected.\r\n");
  } else if (resp == 200 && !fw) {
    totalLength = client.getSize();
    float len = totalLength;
    currentLength = 0;
    uint8_t buff[128] = { 0 };
    WiFiClient * stream = client.getStreamPtr();
    int index = url.lastIndexOf('/');
    String newFileName = url.substring(index + 1);
    String name = WEBPAGE_FOLDER + newFileName;
    ILOG(TAG, "Updating file %s\r\n", newFileName.c_str());
    DLOG(TAG, "File Size: %.0f\r\n", totalLength);
    writeFile(name.c_str(), "");
    while (client.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        String str;
        for (int i = 0; i < c; i++) str += (char)buff[i];
        appendFile(name.c_str(), str.c_str(), false);
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
  } else {
    ELOG(TAG, "Cannot download file. Only HTTP response 200: OK is supported.\r\n");
    ret = -1;
  }
  client.end();
  return ret;
}

void otaFwUpdate(String url, String newVersion) {
  ILOG(TAG, "Updating FW...\r\n");
  if (url.length() == 0 || !url.endsWith(".bin")) {
    WLOG( TAG, "Format of OTA server invalid\r\n");
    return;
  }
  otaUpdate(url, newVersion);
}

int otaWPUpdate(String url, uint8_t type) {
  ILOG(TAG, "Updating web page...%s\r\n", url.c_str());
  if (url.length() == 0 || type == 0 && !url.endsWith(".html") || type == 4 && !url.endsWith(".css") || (type == 1 || type == 2 || type == 3) && !url.endsWith(".js")) {
    WLOG( TAG, "Format of OTA server invalid: %s\r\n", url.c_str());
    return -1;
  }
  return otaUpdate(url, "", type, false);
}

////////////////// publics functions
void otaCheck() {
  ILOG(TAG, "Verify OTA version\r\n");

  client.begin(OTA_CONTROL_URL);
  int resp = client.GET();
  DLOG(TAG, "Response: %i\r\n", resp);
  if (resp == 302) {
    String respStr = client.getString();
    int first = respStr.indexOf("http");
    if (first == -1) return;
    client.end();
    client.begin((respStr.substring(first)).c_str());
    resp = client.GET();
  }
  if (resp == 200) {
    String payload = client.getString();
    String fwVersion = readFile(FW_FILE);
    String wpVersion = readFile(WEB_PAGE_FILE);

    DLOG(TAG, "payload: %s\r\n", payload.c_str());

    DynamicJsonDocument receivedConfig(2048);
    deserializeJson(receivedConfig, payload);

    String receivedWebPageVersion = receivedConfig["webPageVersion"];
    String receivedFwVersion = receivedConfig["fwVersion"];
    DLOG(TAG, "%f : %f\r\n", fwVersion.toFloat(), receivedFwVersion.toFloat());

    if (wpVersion.toFloat() < receivedWebPageVersion.toFloat()) {
      client.end();
      if (
        !otaWPUpdate(receivedConfig["webPageURL0"], 0) &&
        !otaWPUpdate(receivedConfig["webPageURL1"], 1) &&
        !otaWPUpdate(receivedConfig["webPageURL2"], 2) &&
        !otaWPUpdate(receivedConfig["webPageURL3"], 3) &&
        !otaWPUpdate(receivedConfig["webPageURL4"], 4)
      )
        writeFile(WEB_PAGE_FILE, receivedWebPageVersion.c_str());
    }

    if (fwVersion.toFloat() < receivedFwVersion.toFloat()) {
      client.end();
      otaFwUpdate(receivedConfig["fwURL"], receivedFwVersion);
    }
    return;
  } else {
    ELOG(TAG, "Cannot check the OTA control file. Only HTTP response 200: OK is supported.\r\n");
  }
  client.end();
}
