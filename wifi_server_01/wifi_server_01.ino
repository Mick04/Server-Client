/*  Connects to the home WiFi network
 *  Asks some network parameters
 *  Starts WiFi server with fix IP and listens
 *  Receives and sends messages to the client
 *  Communicates: wifi_client_01.ino
 */
#include <SPI.h>
#include <ESP8266WiFi.h>
byte ExternalLedPin = D5;
byte ledPin = 2;
char ssid[] = "Gimp";               // SSID of your home WiFi
char pass[] = "FC7KUNPX";               // password of your home WiFi
WiFiServer server(80);                    

IPAddress ip(192, 168, 1,6 );            // IP address of the server
IPAddress gateway(192,168,1,1);           // gateway of your network
IPAddress subnet(255,255,255,0);          // subnet mask of your network

void setup() {
  Serial.begin(115200);                   // only for debug
  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // starts the server
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());  // some parameters from the network
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());
  pinMode(ExternalLedPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
}

void loop () {
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {

   /**************************************
   *               togle internal LED            *
   **************************************/
    
   
      Serial.println(".");
      String request = client.readStringUntil('\r');    // receives the message from the client
      Serial.print("From client: " + request); //Serial.println(request);
      client.flush();
      client.println("Hi client! No, I am listening.\r"); // sends the answer to the client
     //If (request == ){}
      //digitalWrite(ledPin, HIGH);
       //digitalWrite(ledPin, LOW);  // to show the communication only (inverted logic)
       if(digitalRead(ledPin))
       digitalWrite(ledPin, 0);
    else
       digitalWrite(ledPin, 1);
     
      if(request == "LED OFF "){
        digitalWrite(ExternalLedPin, 0);
        
       // Serial.print("line 51 ");
      }
      else if ("LED ON "){
        digitalWrite(ExternalLedPin, 1);
        
      }
      client.stop();                // tarminates the connection with the client
    }
  }
}

