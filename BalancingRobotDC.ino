//Balancing Robot by Jason Dorweiler http://www.jddorweiler.appspot.com/electronics.html#robot
//  Much of the code is adapted from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1284738418/all
// and http://www.kerrywong.com/2012/03/08/a-self-balancing-robot-i
// this version uses a 6DOF board (http://www.csgshop.com/product.php?id_product=64) and servo motors (https://www.adafruit.com/products/154)

#include <Wire.h>
#include <math.h>
#include <Servo.h>

#define   GYR_Y                 0                              
#define   ACC_Z                 1                              
#define   ACC_Y                 2                              
#define ACC (0x53)    //accelerometer address
#define TO_READ (6)      //num of bytes to read each time (two bytes for each axis)
#define   LINE_END              10                             // \n
#define   SPLIT                 58                             // :
#define CTRL_REG1 0x20 // gyroscope registers
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
Servo servo1,servo2;  // create servo object to control a servo 
int gyroscope_Address = 105; //I2C address of the gyroscope
int   STD_LOOP_TIME  =          9;             
int pinI1=8; // enable left wheel motor
int pinI2=11; // enable left wheel motor
int pinI3=12; // enable right wheel motor
int pinI4=13; // enable right wheel motor
int speedpinA=9;//left motor
int speedpinB=10;//right motor
int sensorValue[3]  = { 0, 0, 0};
int sensorZero[3]  = { 0, 0, 0 }; 
int lastLoopTime = STD_LOOP_TIME;
int lastLoopUsefulTime = STD_LOOP_TIME;
unsigned long loopStartTime = 0;
float   actAngle;                                                  
int ACC_angle;
int GYRO_rate;
int torque1, torque2;
int setPoint = 0;
int drive = 0;
int error = 0;
int updateRate = 5;            //Nr of loop to skip sending and receving info from PC
float motorOffsetL = 1;        //The offset for left motor
float motorOffsetR = 1;        //The offset for right motor
byte buff[TO_READ] ;    //6 bytes buffer for saving data read from the device
char str[512];          //string buffer to transform data before sending it to the serial port
int gain = 0.9;


//********************Change the tuning parameters here**********************
//Setpoint.  Find where the robot is balanced.  
double Setpoint=-3;
//Point where it switches from conservative to agressive 
int gapDist=10;
//Aggressive
double aggK=2, aggKp=15, aggKi=0.6, aggKd=8; 
//Conservative
double consK=2, consKp=5, consKi=0.6, consKd=8;
//***************************************************************************

void setup() {
  Serial.begin(57600);
  pinMode(pinI1,OUTPUT);
  pinMode(pinI2,OUTPUT);
  pinMode(speedpinA,OUTPUT);
  pinMode(pinI3,OUTPUT);
  pinMode(pinI4,OUTPUT);
  pinMode(speedpinB,OUTPUT);
  analogReference(EXTERNAL); 
  Wire.begin();         
  //Turning on the accelerometer
  writeTo(ACC, 0x2D, 0);      
  writeTo(ACC, 0x2D, 16);
  writeTo(ACC, 0x2D, 8);
 
  setupgyroscope(2000); // Configure to  - 250, 500 or 2000 deg/sec  
  delay(100);                                                
}

void loop() {
  if (Serial.available() > 0) {
    Setpoint = Serial.read()-48.0;
    //Serial.print("I received: ");
                Serial.println(Setpoint, DEC);
  }
// ********************* Sensor aquisition & filtering *******************
 
  ACC_angle = getAccAngle(); 
  GYRO_rate = getGyroRate();  
  actAngle = 0.35*kalmanCalculate(ACC_angle, GYRO_rate, lastLoopTime);            // calculate Absolute Angle
  Serial.println(actAngle);
// *********************** PID and motor drive *****************
   double gap = abs(Setpoint-actAngle); //distance away from setpoint
  if(gap<gapDist)
  {  //we're close to setpoint, use conservative tuning parameters
    drive = updatePid(Setpoint, actAngle, consK, consKp, consKi, consKd);
  }
  else
  {
     //we're far from setpoint, use aggressive tuning parameters
    drive =  updatePid(Setpoint, actAngle, aggK, aggKp, aggKi, aggKd);
  }
  if(actAngle>(setPoint-100) && actAngle<(setPoint+100))     Drive_Motor(drive); 
 else                                                     Drive_Motor(0);     // stop motors if situation is hopeless
  
// *********************** loop timing control **************************
  lastLoopUsefulTime = millis()-loopStartTime;
  if(lastLoopUsefulTime<STD_LOOP_TIME)         delay(STD_LOOP_TIME-lastLoopUsefulTime);
  lastLoopTime = millis() - loopStartTime;
  loopStartTime = millis();
}
