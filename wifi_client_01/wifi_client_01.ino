/*  Connects to the home WiFi network
 *  Asks some network parameters
 *  Sends and receives message from the server in every 2 seconds
 *  Communicates: wifi_server_01.ino
 *  this is just some text to show that i have made some changes
 *  more text to see if this branch is deferant to the master
 *  and much************much ********** more
 */ 
#include <SPI.h>
#include <ESP8266WiFi.h>

byte ledPin = 2;

char ssid[] = "Gimp";               // SSID of your home WiFi
char pass[] = "FC7KUNPX";               // password of your home WiFi
char Answer[15] = {0};
unsigned long askTimer = 0;
/***************************************************************
 *                 variables for push button                   *
 *                                                             *
 ***************************************************************/
// constants won't change. They're used here to set pin numbers:
const int buttonPin = D5;     // the number of the pushbutton pin
const int ExternalLedPin =  D6;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
/***************************************************************
 *                 variables for push button end               *
 *                                                             *
 ***************************************************************/

IPAddress server(192,168,1,6);       // the fix IP address of the server
WiFiClient client;

void setup() {
  Serial.begin(115200);               // only for debug
  WiFi.begin(ssid, pass);             // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  /***************************************************************
  *                 settup LED and the button                    *
  ***************************************************************/
    // initialize the LED pin as an output:
    pinMode(ExternalLedPin, OUTPUT);
    // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  /***************************************************************
  *                 settup LED and the button end                *
  ***************************************************************/
 
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());    // Network parameters
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop () {
  client.connect(server, 80);   // Connection to the server
  //digitalWrite(ledPin, LOW);    // to show the communication only (inverted logic)
  Serial.println(".");
  //client.println("Hello server! Are you sleeping?\r");  // sends the message to the server
  client.println(Answer);  // sends the message to the server
  String answer = client.readStringUntil('\r');   // receives the answer from the sever
  Serial.println("from server: " + answer);
  client.flush();
  /**************************************
   *               togle internal LED   *
   **************************************/
   
   if(digitalRead(ledPin))
       digitalWrite(ledPin, 0);
    else
       digitalWrite(ledPin, 1);

  delay(1000);                  // client will trigger the communication after two seconds

   // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ExternalLedPin, HIGH);
    
     strncpy( Answer, "LED ON \r", sizeof(Answer) );
  Answer[sizeof(Answer)-1] = 0;
     Serial.println(Answer);
  } else {
    // turn LED off:
    digitalWrite(ExternalLedPin, LOW);

     strncpy( Answer, "LED OFF \r", sizeof(Answer) );
  Answer[sizeof(Answer)-1] = 0;
     Serial.println(Answer);
  }


}
