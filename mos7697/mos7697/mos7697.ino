#include <LWiFi.h>
#include <LBLE.h>
#include <LBLEPeriphral.h>

#define CLK 5//pins definitions for TM1637 and can be changed to other ports
#define DIO 6

volatile uint32_t lastInterrupt = 0;
volatile uint32_t length = 0;
volatile bool interrupted = 0;
volatile bool eventStart = 0;

uint32_t count = 0;

int status = WL_IDLE_STATUS;
char ssid[] = ""; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

char server[] = "";
//unsigned int localPort = 2390;      // local port to listen on

bool _wasConnected;
String _ssidString;
String _passString;
LBLEService _periphralService("D709A00C-DA1A-4726-A33D-CF62B8F4C3D6");
/*
 * SSID & password must be sent as UTF-8 String and length 
 * must < 20 bytes due to BLE MTU limitation. 
 * 
 * If password length equals to 0, will connect to SSID as open.
 */
LBLECharacteristicString _ssidRead("61DE21BC-6E02-4631-A0A7-1B6C7AF0DAEE", LBLE_WRITE);
LBLECharacteristicString _passRead("B882467F-77BC-4697-9A4A-4F3366BC6C35", LBLE_WRITE);


WiFiClient client;


/*/void pinChanged(){
  
  if(!digitalRead(2)){
    if(millis()-lastInterrupt>100){
      lastInterrupt = millis();
      eventStart = 1;
    }
    
  }
  else{
    if(!interrupted && eventStart==1){
      length = millis()-lastInterrupt;
      interrupted = 1;
      eventStart = 0;
    }
  }

}/*/

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
    }
}

void setup(){
  Serial.begin(9600);
  
  Serial.println("Started");
  LBLE.begin();
  while (!LBLE.ready()) {
    delay(100); 
  }
  _periphralService.addAttribute(_ssidRead);
  _periphralService.addAttribute(_passRead);
  LBLEPeripheral.addService(_periphralService);
  LBLEPeripheral.begin();
  LBLEAdvertisementData _advertisement;
  _advertisement.configAsConnectableDevice("Mosquito Killer22222");
  LBLEPeripheral.advertise(_advertisement);
  Serial.println("BLE Ready!");
  
  
  pinMode(2, INPUT);
  //attachInterrupt(2,pinChanged, CHANGE);
  /*pin 2*/

  // attempt to connect to Wifi network:
  
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  
}


uint32_t pulseStart = 0;
bool pulseEnter = 0;

void loop(){
  /*
    if (_ssidRead.isWritten()) {
        WiFi.disconnect();
        _ssidString = _ssidRead.getValue();
        _passString = "";
        Serial.print("New SSID: ");
        Serial.println(_ssidString);
    }
    if (_passRead.isWritten()) {
        WiFi.disconnect();
        _passString = _passRead.getValue();
        Serial.print("New Password: ");
        Serial.println(_passString);
    }
    if ( ( _ssidString.length() || _passString.length() ) && WiFi.status() != WL_CONNECTED) {
        Serial.print("Connecting Status: ");
        Serial.println(WiFi.status());
        connectWiFi(_ssidString, _passString);
        printWifiStatus();
    }
  */
  if(digitalRead(2)==0){
      while(digitalRead(2)==1){};
      while(digitalRead(2)==0){};
      count++;
      Serial.println(count);

      client.stop();

      // if there's a successful connection:
      if (client.connect(server, 3000)) {
        Serial.println("connecting...");
        // send the HTTP PUT request:
        String jsonStr = "{\"bugZapperId\":123,\"lng\":56.7,\"lat\":23.997916,\"cnt\":" + String(count) + "}";  // 定義JSON字串
        Serial.println(jsonStr);
        client.println("POST /bugZapper/ HTTP/1.1");
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(jsonStr.length());
        client.println();
        client.print(jsonStr);
        } else {
          // if you couldn't make a connection:
          Serial.println("connection failed");
        }
     // interrupted = 0;
  }
  
}

