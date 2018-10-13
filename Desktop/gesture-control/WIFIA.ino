/* This is access point */
#include <ESP8266WiFi.h>
#define PORT 23

const char *ssid = "test_AP";           // SSID of AP
const char *pass = "12344321";         // password of AP
int objArr[10]={0,0,0,0,0,0,0,0,0,0};
int staArr[10]={0,0,0,0,0,0,0,0,0,0};
int staArrIndex=0;
int objArrIndex=0;
int preObj=0;
int pasObj=0;
int preSta=0;
int pasSta=0;
int sendIndex=12;

int avg(int *arr){
  int sum=0;
  for(int i=0;i<10;i++){
    sum+=arr[i];
  }
  if(sum%10>=5)
    return ((sum/10)+1);
  else
    return (sum/10);
}

WiFiServer server(PORT);

IPAddress IP(192,168,4,15);
IPAddress mask = (255, 255, 255, 0);

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  WiFi.softAPConfig(IP, IP, mask);
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  
  if (!client) {
    //Serial.println("client not connected");
    return;
  }
  //Serial.println("client connected");
  client.setTimeout(50);
  String request = client.readStringUntil('\r');
  if(request.length())
    Serial.println(request);
  
    /*if(request.startsWith("helloash")){
      request.remove(0,8);
      String object=request;
      String obj="obj";
      object.remove(1,7);
      String onoff=request;
      onoff.remove(0,7);
    
      int onoffno=onoff.toInt();
      int objectno=object.toInt();
      if(onoffno){
        staArr[staArrIndex++]=onoffno-1;
        staArrIndex%=10;
      }
      if(objectno){
        objArr[objArrIndex++]=objectno-1;
        objArrIndex%=10;
      }
    
      preObj=avg(objArr);
      preSta=avg(staArr);
      if(preObj!=pasObj || preSta!=pasSta){
        sendIndex=24;
      }
      pasObj=preObj;
      pasSta=preSta;
    
      if(sendIndex){
        Serial.print(obj+preObj+"on"+preSta+"\r");
        Serial.flush();
        sendIndex--;
        delay(1);
      }
    }*/
}
