
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event) {
  mqttReconnectTimer.detach();
}

bool startAP(int hid, const char *ssid, const char *password) {
  inAPMode = true;
  WiFi.mode(WIFI_AP);
#ifdef DEBUG
  Serial.print(F("[ INFO ] Configuring access point... "));
#endif

  bool success;
  if (hid == 1) {
    success = WiFi.softAP(ssid, password, 3, true);
  } else {
    success = WiFi.softAP(ssid, password);
  }
#ifdef DEBUG
  Serial.println(success ? "Ready" : "Failed!");
#endif

  if (!success) {
    ESP.restart();
  }

#ifdef DEBUG
  IPAddress myIP = WiFi.softAPIP();

  Serial.print(F("[ INFO ] AP IP address: "));
  Serial.println(myIP);
  Serial.printf("[ INFO ] AP SSID: %s\n", ssid);
#endif
  isWifiConnected = success;
  return success;
}

// Fallback to AP Mode, so we can connect to ESP if there is no Internet connection
void fallbacktoAPMode() {
  inAPMode = true;
#ifdef DEBUG
  Serial.println(F("[ INFO ] ESP-RFID is running in Fallback AP Mode"));
#endif
  uint8_t macAddr[6];
  WiFi.softAPmacAddress(macAddr);
  char ssid[15];
  sprintf(ssid, "ESP-RFID-%02x%02x%02x", macAddr[3], macAddr[4], macAddr[5]);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  isWifiConnected = true;
}


bool connectSTA(const char *ssid, const char *password, byte bssid[6]) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, 0, bssid);

#ifdef DEBUG
  Serial.print(F("[ INFO ] Trying to connect WiFi: "));
  Serial.print(ssid);
#endif
  
  unsigned long now = millis();
  uint8_t timeout = 20; // define when to time out in seconds
  do {
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
    delay(500);

#ifdef DEBUG
    Serial.print(F("."));
#endif

  } while (millis() - now < timeout * 1000);
  
  // We now out of the while loop, either time is out or we connected. check what happened
  if (WiFi.status() == WL_CONNECTED) {
    // Assume time is out first and check
#ifdef DEBUG
    Serial.println();
    Serial.print(F("[ INFO ] Client IP address: "));
    Serial.println(WiFi.localIP());
#endif
    isWifiConnected = true;
    String data = ssid;
    data += " " + WiFi.localIP().toString();
    //writeEvent("INFO", "wifi", "WiFi is connected", data);
    return true;
  } else {
    // We couln't connect, time is out, inform
#ifdef DEBUG
    Serial.println();
    Serial.println(F("[ WARN ] Couldn't connect in time"));
#endif
    return false;
  }
}

void disableWifi() {
  isWifiConnected = false;
  WiFi.disconnect(true);
#ifdef DEBUG
  Serial.println("Turn wifi off.");
#endif
}

void enableWifi() {
#ifdef DEBUG
  Serial.println("[ INFO ] Restarting the board to connect wi-fi again");
#endif
  ESP.restart();
}

