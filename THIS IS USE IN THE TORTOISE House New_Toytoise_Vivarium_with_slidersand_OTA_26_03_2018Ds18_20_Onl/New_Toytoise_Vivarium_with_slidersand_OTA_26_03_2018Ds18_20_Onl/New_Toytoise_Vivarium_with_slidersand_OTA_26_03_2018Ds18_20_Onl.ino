#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <OneWire.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define Relay_Pin D5//active board

OneWire  ds(D7);//active board  // on pin 10 (a 4.7K resistor is necessary)
//WidgetLED led1(V1);
WidgetRTC rtc;
BlynkTimer timer;

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

// Your WiFi credentials.

char ssid[] = "Gimp";
char password[] = "FC7KUNPX";

char auth[] = "2267c1c3b15c41cab120a4be091cef47";//active board

byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
float celsius;
float s1;
float s2;
float s3;
int adr;
int DayHighTemp;
int NightHighTemp;
int LowTemp;
int Days;
int Day_Hours;
int Day_Minutes;
int Day_Settings;
int Night_Hours;
int Night_Minutes;
int Night_Settngs;
int VirtualPin;
bool Am;
bool Reset = V3;
double temp, hum;

 /*************************************************************
 *                           Relay Control                    *
 *                                start                       *
 *************************************************************/

void relay_Control(){
  if (Am == true){
    if(s2 < LowTemp){
      Blynk.setProperty(V11, "color","#FF0000");
      Blynk.setProperty(VirtualPin, "color","#FF0000");
      digitalWrite (Relay_Pin, HIGH);
    }
    else if (s2 > LowTemp){
      Blynk.setProperty(V11, "color","#00FF00");
      Blynk.setProperty(VirtualPin, "color","#00FF00");
      digitalWrite (Relay_Pin, LOW);
    }
  }
    if (Am == false){
      if(s2 < LowTemp){
        Serial.print("  s2 < LowTemp  ");
        Blynk.setProperty(V11, "color","#FF0000");
        Blynk.setProperty(VirtualPin, "color","#FF0000");
        digitalWrite (Relay_Pin, HIGH);
      }
      else if (s2 > LowTemp){
        Blynk.setProperty(V11, "color","#00FF00");
        Blynk.setProperty(VirtualPin, "color","#00FF00");
        digitalWrite (Relay_Pin, LOW);
      }
    }

}
 /*************************************************************
 *                           Relay Control                    *
 *                                End                         *
 *************************************************************/

  /*************************************************************
 * Look For Changes to the Sliders On the settings Tab        *  
 *                          in the Blynk App                  *
 *                                start                       *
 *************************************************************/

BLYNK_WRITE(V3)
{
  Reset = param.asInt(); // if Reset slider is moved to ON then Reset = 1
  if (Reset == 1){       // and the other sliders can be moved
      Blynk.setProperty(V3, "color","#FF0000");
    }
    else {
      Blynk.setProperty(V3, "color","#3700FD");
    }

}
  

BLYNK_WRITE(V4){
  if (Reset == 1){
    Day_Hours = param.asInt(); // assigning incoming value from pin V4 to a variable
    Blynk.virtualWrite(V4, Day_Hours);
  }
}

BLYNK_WRITE(V5){
  if (Reset == 1){
    Day_Minutes = param.asInt(); // assigning incoming value from pin V5 to a variable
    Blynk.virtualWrite(V5, Day_Minutes);
    Blynk.setProperty(V5, "color","#23CBCF");
  }
}
BLYNK_WRITE(V6){
  if (Reset == 1){
    Day_Settings = param.asInt(); // assigning incoming value from pin V6 to a variable
    DayHighTemp = Day_Settings;
    Blynk.virtualWrite(V6, Day_Settings);
    Blynk.setProperty(V6, "color","#00FF00");
  }
}
BLYNK_WRITE(V7){
  if (Reset == 1){
    Night_Hours = param.asInt(); // assigning incoming value from pin V7 to a variable
    Blynk.virtualWrite(V7, Night_Hours);
  }
}
BLYNK_WRITE(V8){
  if (Reset == 1){
    Night_Minutes = param.asInt(); // assigning incoming value from pin V8 to a variable
    Blynk.virtualWrite(V8, Night_Minutes);
    Blynk.setProperty(V8, "color","#23CBCF");
  }
}
BLYNK_WRITE(V9){
  if (Reset == 1){
    Night_Settngs = param.asInt(); // assigning incoming value from pin V9 to a variable
    NightHighTemp = Night_Settngs;
    Blynk.virtualWrite(V9, Night_Settngs);
    Blynk.setProperty(V9, "color","#00FF00");
  }
}


 /*************************************************************
 * Look For Changes to the Sliders On the settings Tab        *  
 *                          in the Blynk App                  *
 *                                End                         *
 *************************************************************/

void sendSensor()
{
  


   /**************************
    *    DS18B20 Sensor      * 
    *      Starts Here       *
    **************************/

  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  for( i = 0; i < 8; i++) {           //we need to drop 8 bytes of data
  }
  adr = (addr[7]);

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad


  for ( i = 0; i < 9; i++) {           // we need 9 bytes to drop off
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }

/*************************************************************
 *                           Heater Control                  *
 *                                start                      *
 ************************************************************/
 
  celsius = (float)raw / 16.0;
  //if(adr == 228)  {        //Other Side
  if(adr == 89)  {        // active board Other Side   
    s1 = (celsius);          

  }
  //if(adr == 197)  {        //Heater Control
  if(adr == 96)  {        //active board  Heater Control
    s2 = (celsius);           //change celsius to fahrenheit if you prefer output in Fahrenheit;
    Am = isAM();
    if(Am == true){
      if(Day_Hours == hour()){  //set LowTemp for the Night time setting
        if (Day_Minutes >= minute() && Day_Minutes <= minute()){
          LowTemp = DayHighTemp -1;
          Blynk.setProperty(V6, "color","#00FF00");
          Blynk.setProperty(V9, "color","#00FF00");
          VirtualPin = V6;
        }
      }
    }
    if(Am == false){
      if(Night_Hours == hour()){  //set LowTemp for the Night time setting
        if(Night_Minutes >= minute()&& Night_Minutes <= minute()+5){
          LowTemp = NightHighTemp -1;
          Blynk.setProperty(V6, "color","#00FF00");
          Blynk.setProperty(V9, "color","#00FF00");
          VirtualPin = V9;
        }
      }
    }
  }
  //if(adr == 92)  {    //Out Side
  if(adr == 116)  {    //active board Out Side
    s3 = (celsius);
  }
  relay_Control();//call relay_Control function
 /*************************************************************
 *                           Heater Control                   *
 *                                  End                       *
 *************************************************************/

  if(Am == true){

    Blynk.virtualWrite(V1, "AM");
  }
  else if(Am == false){
    Blynk.virtualWrite(V1, "PM");
  }
 
  Blynk.virtualWrite(V2, LowTemp +1);
  Blynk.virtualWrite(V4, Day_Hours);
  Blynk.virtualWrite(V5, Day_Minutes);
  Blynk.virtualWrite(V6, Day_Settings);
  Blynk.virtualWrite(V7, Night_Hours);
  Blynk.virtualWrite(V8, Night_Minutes);
  Blynk.virtualWrite(V9, Night_Settngs);
  Blynk.virtualWrite(V10, s1);
  Blynk.virtualWrite(V11, s2);
  Blynk.virtualWrite(V12, s3);
  Blynk.setProperty(V13, "color","#00FF00");
  Blynk.virtualWrite(V13, s2);
}

    /**************************
    *    DS18B20 Sensor       * 
    *      Ends Here          *
    **************************/

void setup() {
  pinMode(Relay_Pin, OUTPUT);
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
    
  }
  
  Blynk.begin(auth, ssid, password);// See the connection status in Serial Monitor

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  timer.setInterval(500, sendSensor);

}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
}
