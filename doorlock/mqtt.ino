
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

  //split payload actions to parts array
  /*char dataBufferParts[sizeof(dataBuf)];
  strncpy(dataBufferParts, dataBuf, sizeof(dataBuf));

  char *payload_parts[3];
  int loopIndex = 0;
  
  char* payload_part = strtok(dataBufferParts, "/");
  payload_parts[loopIndex] = payload_part;
  loopIndex = loopIndex + 1;
  while(payload_part != NULL && loopIndex < 3) {
    payload_part = strtok(NULL, "/");
    payload_parts[loopIndex] = payload_part;
    loopIndex = loopIndex + 1;
  }*/

  String topicStr = topicBuf;
  String dataStr = dataBuf;
  if(topicStr == COMMAND_TOPIC) {
    if (dataStr == ACTION_DOOR_UNLOCK) {
      openDoor();
    }
    else if (dataStr == ACTION_DOOR_LOCK) {
      closeDoor();
    }
  }

  //check for actions
  /*if (payload_parts != NULL && sizeof(payload_parts) >= 3 && strcmp(payload_parts[0], SUB_PREFIX)) {

    //door action
    if (strcmp(payload_parts[1], DOOR_TOPIC)) {
      if (strcmp(payload_parts[2], ACTION_DOOR_OPEN)) {
        //open door for 2 seconds
      }
      else if (strcmp(payload_parts[2], ACTION_DOOR_CLOSE)) {
        
      }
    }
  }*/
}

boolean reconnect() {
  String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
  if (clientMqtt.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
    clientMqtt.subscribe(COMMAND_TOPIC);
    clientMqtt.publish(STATE_TOPIC, ACTION_DOOR_LOCK);
  }
  return clientMqtt.connected();
}

