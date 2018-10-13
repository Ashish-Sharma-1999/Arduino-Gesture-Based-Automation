 
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)

MPU6050 mpu;

// Unccomment if you are using an Arduino-Style Board
 #define ARDUINO_BOARD

#define LED_PIN 13      // (Galileo/Arduino is 13)
#define SETPIN 5
#define WIFIPIN 7
#define LIMIT 5
#define THUMB A0
/*#define INDEXFINGER A1
#define MIDDLEFINGER A2
#define RINGFINGER A3
#define SHORTFINGER A6*/
#define ENGAGESTA 8
#define ADDMODESTA 6
#define FOUNDSTA 9

int yy,pp,rr;
byte index=0;
//defination of clas object
class object{ 
  int y,p,r;          //yaw,pitch,role
  static int yawlimit;
  static int pchlimit;
  public:
    byte onoff;     //true if on
    
    object(){         //constructor
      y=0;
      p=0;
      r=0;
      onoff=0;
    } 
      
    void assign(object* obj,float ypr[3],int ind){  //set object
      y=ypr[0];
      p=ypr[2];
      for(int i=0;i<ind;i++){
        yy=abs(y-obj[i].y);
        if(yy>180){
          yy=360-yy;   
        }
        Serial.println(yy);
        if(yy<yawlimit){
          if(yy>LIMIT)
            yawlimit=yy;
          else{
            pp=abs(p-obj[i].p);
            if(pp<pchlimit && pp>LIMIT){
              pchlimit=pp;
            }
          }
        }
      }
    }
        
    boolean compare(float ypr[3]){    //comapare ypr and returns true if within limit
      pp=ypr[2]-p;
      yy=ypr[0]-y;
      if(yy>-yawlimit && yy<yawlimit){
        if(pp>-pchlimit && pp<pchlimit)
          return true;
        else
          return false;
      }
      else
        return false;
    }
};
int object::yawlimit=90;
int object::pchlimit=45;
// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
boolean engaged=false;  // not engaged initally
boolean found=false;    // if true finger command will be read
boolean addmode=true;   //add mode is initially on to add objects
//boolean adprestate=false,adpasstate=false;   //to store present and past state of ADDPIN
boolean stprestate=false,stpasstate=false;   //to store present and past state of SETPIN
boolean wifi=true;     //wifi will be on
boolean sendRequest=false;    //to check request is checked or not
int objectno=0;
boolean thumb=false,indexfinger=false,middlefinger=false,ringfinger=false,shortfinger=false;
VectorFloat gravity;    // [x, y, z]            gravity vector
Quaternion q;           // [w, x, y, z]         quaternion container
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
object obj[4];
byte i=0,j=0;
int k=0,f=10;
int fingerSelector=0;
String request="ash";
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SETPIN, INPUT);
  pinMode(WIFIPIN,OUTPUT);
  pinMode(THUMB,INPUT);
  /*pinMode(INDEXFINGER,INPUT);
  pinMode(MIDDLEFINGER,INPUT);
  pinMode(RINGFINGER,INPUT);
  pinMode(SHORTFINGER,INPUT);*/
  pinMode(ADDMODESTA,OUTPUT);
  pinMode(ENGAGESTA,OUTPUT);
  pinMode(FOUNDSTA,OUTPUT);
  digitalWrite(WIFIPIN,HIGH);
  digitalWrite(ENGAGESTA,HIGH);
  digitalWrite(ADDMODESTA,HIGH);
  digitalWrite(FOUNDSTA,HIGH);
  delay(4000);
  digitalWrite(ENGAGESTA,LOW);
  digitalWrite(ADDMODESTA,LOW);
  digitalWrite(FOUNDSTA,LOW);
        // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
        //  int TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  Serial.begin(115200);    //for transfering data to wifi module
        // initialize MPU-6050
  mpu.initialize();
        // verify connection
  if(!(mpu.testConnection())){
    digitalWrite(LED_PIN,HIGH);
    delay(2000);
    digitalWrite(LED_PIN,LOW);
  }
        // load and configure the DMP
  devStatus = mpu.dmpInitialize();
        // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

        // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
        // turn on the DMP, now that it's ready
    mpu.setDMPEnabled(true);
        // enable Arduino interrupt detection
    attachInterrupt(digitalPinToInterrupt(3), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
        // set our DMP Ready flag so the main loop() function knows it's okay to use it
    dmpReady = true;
        // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } 
  while (!digitalRead(SETPIN)==HIGH);
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
      // if programming failed, don't try to do anything
  if (!dmpReady) return;
      // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize);
      // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
      // get current FIFO count
  fifoCount = mpu.getFIFOCount();
      // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
  }
      // otherwise, check for DMP data ready interrupt (this should happen frequently)
  else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            
    ypr[0]=ypr[0]*57.3248;  //converting to degrees
    ypr[1]=ypr[1]*57.3248;
    ypr[2]=ypr[2]*57.3248;
    /*Serial.print(ypr[0]);
    Serial.print("\t");
    Serial.print(ypr[1]);
    Serial.print("\t");
    Serial.println(ypr[2]);*/
    if(addmode){   //used to add new object
      digitalWrite(ADDMODESTA,HIGH);
      stprestate=digitalRead(SETPIN);
      if(stpasstate && !stprestate){
        if(analogRead(THUMB)<=300){
          obj[index].assign(obj,ypr,index);
          objectno=index;
          digitalWrite(ENGAGESTA,HIGH);
          for(int a=0;a<10;a++){
            Serial.print(request + (objectno+1) + "sta2" + "\r");
          }
          delay(1000);
          sendRequest=true;
          index++;
          digitalWrite(ENGAGESTA,LOW);
          f=10;
        }
        else
          addmode=false;          //new object cant be added
      }
      stpasstate=stprestate;     //to remember the past state
    }
    
    else {         //used to check whethere any object is pointed or not
      digitalWrite(ADDMODESTA,LOW);
      if(!found){         //if no object found then find an object
        digitalWrite(FOUNDSTA,LOW);
        if(engaged){      //is setup pin is pressed engaged will be true and objects will be compared
          digitalWrite(WIFIPIN,HIGH);
          for(i=0;i<=index;i++){
            if(obj[i].compare(ypr)){  
              found=true;               //flag is set when object is found
              objectno=i;
              sendRequest=true;                  
              f=5;
              break;                    //once an object is found no further comparision will take place
            }     
          }
        }
      }
      
      if(engaged)
        digitalWrite(ENGAGESTA,HIGH);   //led to show engaged status //engaged
      else 
        digitalWrite(ENGAGESTA,LOW);           //not engaged
      
      if(found && engaged){
        digitalWrite(FOUNDSTA,HIGH);    //once object is found and engaed is on further processing will be done
        ((analogRead(THUMB)>=300) ? thumb=true: thumb=false);  //checks flex sensor value
        if(thumb) {
          (obj[objectno].onoff ? obj[objectno].onoff=0 : obj[objectno].onoff=1);        //once object is turned on or off 
          engaged=false;                     //engaged is turned off
          f=10;                              //how many times send request 
        }
      }
      else{ 
        found=false;                         //no object is engaged
      }          
      stprestate=digitalRead(SETPIN);
      if(stprestate && (!stpasstate))         //if setpin alter its state then engaged is altered
        engaged=!engaged;                     //from high to low
      stpasstate=stprestate;
    }
  
    if(index>=4)       //maximum objects are added 
      addmode=false;      //no further addition can take place
      
    if(f){               //index for sending request
      Serial.print(request+(objectno+1)+"sta"+((obj[objectno].onoff)+1)+"\r");
      f--;
    }
  }
  digitalWrite(WIFIPIN,HIGH);
}

