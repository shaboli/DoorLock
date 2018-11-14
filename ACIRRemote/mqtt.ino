
void myDataCb(String& topic, String& data);
void myPublishedCb();
void myDisconnectedCb();
void myConnectedCb();

long lastReconnectAttempt = 0;

void setupMQTT() {
  clientMqtt.setServer(mqtt_server, 1883);
  clientMqtt.setCallback(callback);
}

void loopMQTT() {
  if (!clientMqtt.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        Serial.println("MQTT connected");
        lastReconnectAttempt = 0;
      }
      else {
        Serial.println("MQTT failed to connect");
      }
    }
  } else {
    // Client connected
    clientMqtt.loop();
  }
}

boolean reconnect() {
  String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
  if (clientMqtt.connect(clientId.c_str())) {
    clientMqtt.subscribe(COMMAND_SEND_TOPIC);
  }
  return clientMqtt.connected();
}

void callback(char* topic, byte* payload, unsigned int payload_length) {
  // Allocate the correct amount of memory for the payload copy
  char dataBuf[payload_length + 1], topicBuf[60];
  dataBuf[payload_length] = 0;
  
  strncpy(topicBuf, topic, sizeof(topicBuf));
  memcpy(dataBuf, payload, sizeof(dataBuf));
  dataBuf[sizeof(dataBuf) - 1] = 0;

#ifdef DEBUG
  Serial.print("Message arrived [");
  Serial.print(topicBuf);
  Serial.print("] ");
  Serial.print(dataBuf);
  Serial.println();
#endif

  String topicStr = topicBuf;
  String dataStr = dataBuf;
  Serial.println("Topic:");
  Serial.print(topicStr);
  if(topicStr == COMMAND_SEND_TOPIC) {
    sendIRString(dataBuf);
  }
}

