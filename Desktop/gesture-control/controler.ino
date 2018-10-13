#include<SoftwareSerial.h>
int pin=2;
int i=0;
int j=0;
int preobj=0;
int pasobj=0;
int presta=0;
int passta=0;
struct objects{
  int pinNo;
  bool onoff;

  objects(){
    pinNo=pin++;
    onoff=false;
  }

  void update(){
    digitalWrite(pinNo,onoff);
  }
};

objects obj[4];
SoftwareSerial mySerial(10,11);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.setTimeout(100);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(mySerial.available()){
    String request=mySerial.readStringUntil('\r');
    //Serial.println(request+"received");
    //mySerial.read();
    if(request.startsWith("ash")){
      request.remove(0,3);
      String object=request;
      object.remove(1,4);
      preobj=object.toInt()-1;
      request.remove(0,4);
      presta=request.toInt()-1;
      if(preobj==pasobj && presta==passta)
        j++;
      else
        j=0;
      pasobj=preobj;
      passta=presta;
    }
  if(j>=4){
    obj[preobj].onoff=presta;
    Serial.print("obj");
    Serial.print(preobj);
    Serial.print("on");
    Serial.println(presta);
  }
 }
  obj[i++].update();
  i%=4;
} 
