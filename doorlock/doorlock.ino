#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <Ticker.h>
#include <WiFiManager.h>

//COMMENT THIS TO TURN OFF SERIAL PRINTS
//#define DEBUG
//COMMENT THIS TO TURN OFF SERVO SUPPORT
//#define SERVO

#define mqtt_server             "192.168.1.62"
#define COMMAND_TOPIC           "house/doorlock/set"
#define STATE_TOPIC             "house/doorlock/"
#define ACTION_DOOR_UNLOCK      "unlock"
#define ACTION_DOOR_LOCK        "lock"
const char* mqtt_user = "shaboli";
const char* mqtt_password = "shabo12345";

const int espLed = LED_BUILTIN;

const char* APssid = "$doorLock";
const char* APpassword = "12345678";

const char* ssid = "Shabo";
const char* password = "0507432485";

ESP8266WebServer server(80);

//Wifi
bool inAPMode = false;
bool isWifiConnected = false;
WiFiClient espClient;
WiFiEventHandler wifiDisconnectHandler;

//Mqtt
PubSubClient clientMqtt(espClient);
Ticker mqttReconnectTimer;

#define DOOR_SERVO_PIN 5
#define DOOR_SWITCH_PIN 4

//Door
Servo doorServo;
bool openDoorInProgress = false;
long doorOpenTime = 0;
const long doorOpenInterval = 500;

void setup(void) {
  pinMode(espLed, OUTPUT);
  pinMode(DOOR_SWITCH_PIN, OUTPUT);
  
  digitalWrite(espLed, LOW);

  Serial.begin(115200);
  
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

#ifdef SERVO
  doorServo.attach(DOOR_SERVO_PIN);
#endif

  setupWifi();
  setupServer();
  setupMQTT();
}

void loop(void) {
  server.handleClient();

  loopMQTT();
  loopDoor();
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

  /*wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("doorLock")) {
    Serial.println("MDNS responder started");
  }*/
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
/////////////DOOR/////////////////
//////////////////////////////////

void loopDoor() {
  if(openDoorInProgress) {
    long now = millis();
    if (now - doorOpenTime > doorOpenInterval) {
        closeDoor();
    }
  }
}

void openDoor() {
  // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  digitalWrite(espLed, HIGH);
  digitalWrite(DOOR_SWITCH_PIN, HIGH);

#ifdef SERVO
  doorServo.write(180);
#endif
  
  clientMqtt.publish(STATE_TOPIC, ACTION_DOOR_UNLOCK);
  doorOpenTime = millis();
  openDoorInProgress = true;

#ifdef DEBUG
  Serial.println("Open Door");
#endif
}

void closeDoor() {
  digitalWrite(espLed, LOW);
  digitalWrite(DOOR_SWITCH_PIN, LOW);

#ifdef SERVO
  doorServo.write(0);
#endif

  clientMqtt.publish(STATE_TOPIC, ACTION_DOOR_LOCK);
  openDoorInProgress = false;
  
#ifdef DEBUG
  Serial.println("Close Door");
#endif
}

