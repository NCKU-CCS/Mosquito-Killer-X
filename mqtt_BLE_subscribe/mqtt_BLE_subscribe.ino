#include <LWiFi.h>
#include <LBLE.h>
#include <LBLEPeriphral.h>
#include <MQTTClient.h>


#define CLK 5//pins definitions for TM1637 and can be changed to other ports
#define DIO 6

volatile uint32_t lastInterrupt = 0;
volatile uint32_t length = 0;
volatile bool interrupted = 0;
volatile bool eventStart = 0;

uint32_t count = 0;

int status = WL_IDLE_STATUS;

char mqtt_server[] = "";
char Topic[] = "";
char SubTopic[] = "";
char publish_json[100];
//unsigned int localPort = 2390;      // local port to listen on

bool wasReceived = false;
/*
 * SSID & password must be sent as UTF-8 String and length 
 * must < 20 bytes due to BLE MTU limitation. 
 * bugZapperId
 * If password length equals to 0, will connect to SSID as open.
 */
String ssidString = "";
String passString = "";
String bugZapperId = "";

LBLEService periphralService("19B10010-E8F2-537E-4F6C-D104768A1214");
LBLECharacteristicString ssidCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", LBLE_WRITE);
LBLECharacteristicString passCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", LBLE_WRITE);
LBLECharacteristicString connetedCharacteristic("19B10013-E8F2-537E-4F6C-D104768A1214", LBLE_READ);

WiFiClient client;
MQTTClient mqtt_client(256);

String jsonStr;

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectWiFi(const String ssidString, const String passString)
{
  if (ssidString.length()) {
        const int ssidLen = ssidString.length() + 1;
        const int passLen = passString.length() + 1;
        char ssidCString[ssidLen];
        char passCString[passLen];
        ssidString.toCharArray(ssidCString, ssidLen);
        passString.toCharArray(passCString, passLen);
        
        Serial.print("Connecting to: [");
        Serial.print(String(ssidCString));
        if (passString.length()) {
            Serial.print("] [");
            Serial.print(String(passCString));
        }
        Serial.println("]");
        if (passString.length()) {
            WiFi.begin(ssidCString, passCString);
        } else {
            WiFi.begin(ssidCString);
        }
        if (WiFi.status() == WL_CONNECTED) {
          mqtt_client.begin(mqtt_server, 1883, client);
          mqtt_client.onMessage(messageReceived);
          Serial.print("connect succeed");
          connect();
          connetedCharacteristic.setValue("connected");
        }
        else {
          Serial.print("connect fail");
          connetedCharacteristic.setValue("failed");
        }
    }
}

void setup(){
  Serial.begin(9600);
  
  Serial.println("Started");
  LBLE.begin();
  while (!LBLE.ready()) {
    delay(100); 
  }
  Serial.println(LBLE.getDeviceAddress());
  periphralService.addAttribute(ssidCharacteristic);
  periphralService.addAttribute(passCharacteristic);
  periphralService.addAttribute(connetedCharacteristic);
  LBLEPeripheral.addService(periphralService);
  LBLEPeripheral.begin();
  LBLEAdvertisementData advertisement;
  advertisement.configAsConnectableDevice(bugZapperId.c_str());
  LBLEPeripheral.advertise(advertisement);
  Serial.println("BLE Ready!");
  
  
  pinMode(2, INPUT);
  randomSeed(analogRead(0));
  connectWiFi(ssidString, passString);
}


void connect() {
  String clientId = String(random(0xffff),HEX);
  while (!mqtt_client.connect(clientId.c_str())) {
    delay(1000);
  }
  mqtt_client.subscribe(SubTopic);
}

uint32_t pulseStart = 0;
bool pulseEnter = 0;
unsigned long lastMillis = 0;

void loop(){

   if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    mqtt_client.loop();
    if (!mqtt_client.connected()) {
      connect();
    }
   }
   if (WiFi.status() == WL_CONNECTED ) {
      if(digitalRead(2)==0) {
        while(digitalRead(2)==1){};
        while(digitalRead(2)==0){};
        count++;
        Serial.println(count);
      
        // if there's a successful connection:
        mqtt_client.loop();
        while(!mqtt_client.connected()) {
          connect();
        }
        jsonStr = "{\"id\":\"" + bugZapperId + "\",\"cnt\":" + String(count) + "}";  // 定義JSON字串
        Serial.println("publish success");
        jsonStr.toCharArray(publish_json, 100);
        mqtt_client.publish(Topic, publish_json, false, 1);
        mqtt_client.loop();
        if (!mqtt_client.connected()) {
          connect();
        }
      }
      if (wasReceived == true) {
        wasReceived = false;
        Serial.println("recieved the message");
        mqtt_client.loop();
        while(!mqtt_client.connected()) {
          connect();
        }
        jsonStr = "{\"id\":\"" + bugZapperId + "\"}";  // 定義JSON字串
        Serial.println("publish success");
        jsonStr.toCharArray(publish_json, 100);
        mqtt_client.publish(Topic, publish_json, false, 1);
        mqtt_client.loop();
        if (!mqtt_client.connected()) {
          connect();
        }
      }
  }
  if(ssidCharacteristic.isWritten()) {
    ssidString = ssidCharacteristic.getValue();
    ssidCharacteristic.setValue("");
    Serial.print("ssid="); Serial.println(ssidString);
    connectWiFi(ssidString, passString);
  }
  if(passCharacteristic.isWritten()) {
    passString = passCharacteristic.getValue();
    passCharacteristic.setValue("");
    Serial.print("pass="); Serial.println(passString);
    connectWiFi(ssidString, passString);
  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
