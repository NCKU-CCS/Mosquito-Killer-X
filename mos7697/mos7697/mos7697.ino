#include <LWiFi.h>
#include <WiFiUdp.h>

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



WiFiClient client;


void pinChanged(){
  
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

}

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
/*
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /latest.txt HTTP/1.1");
    client.println("Host: www.arduino.cc");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();

  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
*/

void setup(){
  Serial.begin(9600);
  
  Serial.println("Started");

  pinMode(2, INPUT);
  attachInterrupt(2,pinChanged, CHANGE);
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
  
  if(interrupted){
      count++;
      Serial.println(count);

      client.stop();

      // if there's a successful connection:
      if (client.connect(server, 3000)) {
        Serial.println("connecting...");
        // send the HTTP PUT request:
        String jsonStr = "{\"bugZapperId\":210,\"lng\":56.7,\"lat\":23.997916,\"cnt\":" + String(count) + "}";  // 定義JSON字串
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
      interrupted = 0;
  }
  
}
