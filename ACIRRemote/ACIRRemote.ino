#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "base64.hpp"

//COMMENT THIS TO TURN OFF SERIAL PRINTS
#define DEBUG

#define mqtt_server             "192.168.1.62"
#define mqtt_user               "shaboli"
#define mqtt_password           "shabo12345"
#define COMMAND_SEND_TOPIC           "cmnd/ir/send"
#define STATE_TOPIC             "house/ir/"
#define ACTION_COMMAND      "send"

const int espLed = 2;//new ESP-12E GPIO2

const char* APssid = "$IRremote";
const char* APpassword = "12345678";

const char* ssid = "Shabo";
const char* password = "0507432485";

ESP8266WebServer server(80);

bool inAPMode = false;
bool isWifiConnected = false;
WiFiClient espClient;
WiFiEventHandler wifiDisconnectHandler;
PubSubClient clientMqtt(espClient);
Ticker mqttReconnectTimer;

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
const int khz = 38; // 38kHz carrier frequency for the NEC protocol
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

void setup(void) {
  pinMode(espLed, OUTPUT);
  digitalWrite(espLed, LOW);

  Serial.begin(115200);
  
  irsend.begin();
  
#ifdef DEBUG
  Serial.println();
  Serial.println(F("[ INFO ] ESP RFID v0.8"));
  
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
  Serial.printf("Flash real size: %u\n\n", realSize);
  Serial.printf("Flash ide  size: %u\n", ideSize);
  Serial.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
  Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  if (ideSize != realSize) {
    Serial.println("Flash Chip configuration wrong!\n");
  } else {
    Serial.println("Flash Chip configuration ok.\n");
  }
#endif

  setupWifi();
  setupServer();
  setupMQTT();
}

void loop(void) {
  server.handleClient();

  loopMQTT();

  //sendIRString("4580,4496,596,1688,592,1660,572,1684,572,528,600,528,628,500,600,528,600,528,600,1684,568,1688,592,1660,572,532,600,528,620,508,596,532,596,532,624,1656,596,1660,568,1684,572,532,600,528,600,528,600,528,620,508,596,532,596,532,624,504,600,1680,568,1688,568,1688,596,1660,568,1684,572");
  //delay(5000);
}

//////////////////////////////////
/////////////WIFI/////////////////
//////////////////////////////////

void setupWifi() {
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect(APssid,APpassword)) {

  #ifdef DEBUG
    Serial.println("failed to connect and hit timeout");
  #endif
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void startAP() {
  WiFiManager wifiManager;
  wifiManager.startConfigPortal(APssid);
}

//////////////////////////////////
///////////////IR/////////////////
//////////////////////////////////

void sendIRCommand(char* base64) {
  //CESoYIwEjCCMIIx8jASMIIwgiICk7IwgjHyk7IwgiCSIJIgkiCSIIIwgjCCMfKTsjCCMIIgkiCSIJIiApOyIgKTsjCCIgKTsiJioAIwgjCCMIIgkjCCIJIgkiCCMIIwgjCCMIIgkiCSMIIgkiCSIIIwgjCCMIIwgiCSIJIggjCSIIIwgjCCMfJT8jA==
  //uint16_t irSignal[] = {8968, 4420, 680, 1632, 652, 516, 652, 544, 652, 544, 652, 1660, 652, 516, 652, 544, 652, 544, 648, 1664, 676, 492, 652, 544, 652, 1660, 676, 492, 652, 544, 648, 548, 648, 548, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 1660, 676, 492, 652, 544, 652, 544, 648, 548, 648, 548, 648, 548, 648, 1664, 676, 492, 648, 1664, 676, 492, 652, 544, 648, 1664, 676, 492, 648, 19864, 680, 512, 652, 544, 652, 544, 652, 544, 648, 548, 652, 544, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 544, 652, 544, 648, 548, 648, 548, 652, 544, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 544, 652, 544, 648, 548, 648, 548, 648, 544, 652, 548, 648, 544, 652, 544, 652, 544, 652, 1660, 660, 508, 652};
  //irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.

  Serial.println(" Data:");
  Serial.print(base64);

  unsigned int binary_length = decode_base64_length((unsigned char *)base64);
  unsigned char irSignal[binary_length];
  decode_base64((unsigned char *)base64, irSignal);

  uint16_t irSignalInt[binary_length];
  for (size_t i = 0; i < binary_length; ++i) {
      irSignalInt[i] = atoi(reinterpret_cast<const char*>(&irSignal[i]));
  }
  
#ifdef DEBUG
  Serial.print("[");
  for (int i=0; i < binary_length; i++){
      Serial.print(irSignalInt[i]);
      Serial.print(","); 
  }
  Serial.print("]\n");
  printf("[[%d]]\n", binary_length);
#endif
  
  irsend.sendRaw(irSignalInt, sizeof(irSignalInt) / sizeof(irSignalInt[0]), khz); //Note the approach used to automatically calculate the size of the array.

  delay(1000);

  digitalWrite(espLed, LOW);
}

void sendIRString(char* rawData) {

  digitalWrite(espLed, HIGH);

  int totalSize = commandTotalSize(rawData);  
  uint16_t blawData[totalSize];
  char *string = strdup(rawData);
  char *token = strtok(string, ",");
  int i = 0;
  while (token) {
      blawData[i++] = atoi(token);
      token = strtok(NULL, ",");
  }
  free(string);

#ifdef DEBUG
  Serial.print("[");
  for (int i=0; i < totalSize; i++){
      Serial.print(blawData[i]);
      Serial.print(","); 
  }
  Serial.print("]\n");
  printf("[[%d]]\n", totalSize);
#endif

  irsend.sendRaw(blawData, sizeof(blawData) / sizeof(blawData[0]), khz); //Note the approach used to automatically calculate the size of the array.

  delay(1000);

  digitalWrite(espLed, LOW);
}


int commandTotalSize(char* rawData) {

  char *string = strdup(rawData);
  char *token = strtok(string, ",");
  int count = 0;
  while (token) {
      count++;
      token = strtok(NULL, ",");
  }
  free(string);
  return count;
}





