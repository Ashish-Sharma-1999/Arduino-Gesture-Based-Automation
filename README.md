# GESTURE-BASED-AUTOMATION-
The “Gesture Based Automation” is a project dedicated for giving the user a control over all the devices by using some very basic sensors
(like flex sensor, accelerometer) and a micro controller. There are 2 main section in this project, one is the transmitting section and 
the other one is the receiving end. The transmitting part is the hand from where we send the signals to an object (basically the receiving
end) with the help of Wi-Fi module (i.e. Esp-01 that belongs to Esp8266 family) that is attached on both the ends. There are two types 
of micro controller one is the Arduino nano (used in hand part) and the other one is the Arduino Uno (used in receiving end). The hand
part is very important as it first find the object by pointing towards the object. Once the object is pointed out, we give the command of 
locking it by using thumb and forefinger connection, 1 led would glow up in response to this action(on hand part).  The object’s roll,
pitch, yaw helps in keeping the location of object. Once the object is found, the receiving end give the signal that it has successfully
connected by blinking the led light after this if the correct object is blinking the led then we lock it. To give the command to a locked
object we would simply fold the forefinger where flex sensor is used.
 In the video we have first of all locked 4 different objects in 4 different direction. The 4 led on the receiver end(on the bread board)
 represent the 4 different object’s  led. We are locking them one by one. Once all the objects are locked, we will randomly point towards 
 these direction where the objects are locked i.e. left, right, middle and upward. Now suppose we have previously locked an object in the 
 middle, the transmitting part(hand part) have 3 different kinds of leds, on finding an object that has been locked previously in the 
 middle, 3 out of 2 leds will glow up until you give that object a command or you change your direction (we have given the command of 
 “ON” to all the 4 objects one by one by folding the forefinger). The next operation that we have performed is giving the command of 
 “OFF” by again pointing towards the locked objects. We are turning them off one by one, by again folding the finger.  
