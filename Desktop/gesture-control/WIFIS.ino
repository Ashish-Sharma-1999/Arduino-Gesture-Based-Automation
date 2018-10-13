/* This is station*/

#include <ESP8266WiFi.h>
#define PORT 23

const char *ssid = "test_AP";           // SSID of AP
const char *pass = "12344321";         // password o AP

long indexa=0;

IPAddress server(192,168,4,15);     // IP address of the AP

WiFiClient *client=new WiFiClient;
WiFiClient *old=NULL;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);           // connects to the WiFi AP
  Serial.setTimeout(30);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}

void loop() {
  while(!client->connect(server, PORT));
  indexa++;
  if(Serial.available()){
    String request = Serial.readStringUntil('\r');
    //client->print(indexa);
    client->print(request+"\r");
    delay(30);
    client->print(request+"\r");
    delay(30);
    client->print(request+"\r");
    delay(30);
    client->print(request+"\r");
  }
  delay(50);
  
  if(indexa>600){
    old=client;
    client=new WiFiClient;
    indexa=0;
  }
  
  if(old && indexa>300){
    delete old;
  }
}
