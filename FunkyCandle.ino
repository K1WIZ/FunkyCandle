//
// Created by: John Rogers 
// Date: Oct 6, 2019
// Permission is granted to freely use and distribute so long as this notice is 
// left in code. License: GPL
//
// john at wizworks dot net
//
#include <ESP8266WiFi.h>          
#include <ESP8266TrueRandom.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>       
#include <EEPROM.h>
MDNSResponder mdns;
ESP8266WebServer server(80);

int state;
int lastState;
String response;  // we build the string that creates the web UI based on selection

void handleRoot() {
  server.send(200, "text/html", response);
}

#define fPin 12
#define fPin2 13

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
    pinMode(fPin, OUTPUT);
    pinMode(fPin2, OUTPUT);
    Serial.begin(115200);
    EEPROM.begin(8);
    WiFiManager wifiManager;
    wifiManager.autoConnect("FunkyCandle");
    WiFi.hostname("candle");
    server.on("/", handleRoot);
  
  server.on("/on", [](){
  server.send(200, "text/html", response);
  state = 1;
  setting();
  });
  
  server.on("/off", [](){
  server.send(200, "text/html", response);
  state = 0;
  setting();
  });

  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void setting(){
  if( EEPROM.read(0) != state ){
    EEPROM.write(0, state);
    EEPROM.commit();
  }
}

void showstate(){
  if( EEPROM.read(0) == 1 ){
    response = "<h1>Wireless Candle</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a><br><br>CANDLE IS ON</p>";
  } else if ( EEPROM.read(0) == 0 ){
    response = "<h1>Wireless Candle</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a><br><br>CANDLE IS OFF</p>";;
  }
}

void candle() {
analogWrite(fPin, ESP8266TrueRandom.random(30,255)); //Use the same pins used above
analogWrite(fPin2, ESP8266TrueRandom.random(30,255));
delay(ESP8266TrueRandom.random(40,70)); //Limits the speed.
}

void idle(){
  lastState = EEPROM.read(0);
  
  if (lastState == 1) {
  candle();
  } else if (lastState == 0) {
    analogWrite(fPin, 0);
    analogWrite(fPin2, 0);
    }
}

void loop() {
      showstate();
      idle();
      server.handleClient();
}
