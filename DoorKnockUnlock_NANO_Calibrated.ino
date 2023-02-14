// MPU-6050 Short Example Sketch
//www.elegoo.com
//2016.12.9

// This version is for NANO (Use NANO and Old Bootloader)
//Servo Wiring: 5V to RED, D9 PMW to YELLOW, GND to BROWN
//Accelerometer Wiring: 3.3 to VCC, GND to GND, A5 to SCL, A4 to SDA

#include<Wire.h>
#include <Servo.h> //library for servo motor
Servo myServo; // naming the servo
int servoBasePin = 2; //for controlling servo's transistor (saving power while off)

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
void setup(){
  myServo.attach(9); //which pin for servo to use
  myServo.write(20);
  pinMode(servoBasePin, OUTPUT);
  digitalWrite(servoBasePin, LOW); // ensures power to servo is off
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

}
int delayKnocks = 150; // how long to wait between each knock.
int knockValues[]{3,3,3,3,3,3,3,3,3,3};
int accelRunTime = 0;
int differenceInValues = 0;
int AcX1 = 0;
int AcX2 = 0;
int passOne[]{0,1,1,1,0,0,0,0,0,0}; // the secret lock pattern

void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();      
  AcX1 = AcX;                          // stores value one
  //Serial.print("AcX = "); Serial.println(AcX);
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();      // stores value one
  AcX2= AcX;    // stores value two
  
  differenceInValues=abs(AcX1 - AcX2);   // stores the difference in both values to determine a knock.
if ((differenceInValues) > 450){
  Serial.print("First Knock & difference is: ");
  Serial.println(differenceInValues);
  differenceInValues=0;
  accelRunTime=0;
  delay(250); // to wait-out noise
  
  while (accelRunTime!=5){
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
    AcX=Wire.read()<<8|Wire.read();      
    AcX1 = AcX;                          // stores value one
    //Serial.print("AcX = "); Serial.println(AcX);

    int ii=0;
    while (ii<200){ //allows time to ceate a knock
      Wire.beginTransmission(MPU_addr);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
      AcX=Wire.read()<<8|Wire.read();      // stores value one
      if (ii==0){ // resets the second AcX value, otherwise the value may be left over from last loop
        AcX2=AcX;
      }
      if (abs(AcX)>abs(AcX2)){ // stores the greatest difference so we don't lose the knock
        AcX2=AcX;    // stores value two
        differenceInValues=abs(AcX1 - AcX2);}
        ii+=1;}
        
    if (differenceInValues>300){
      knockValues[accelRunTime]=1;
      Serial.println("_1");
    }
      else{
      knockValues[accelRunTime]=0;
      Serial.print("_0 & difference is: ");
      Serial.println(differenceInValues);
    }
     accelRunTime+=1; differenceInValues=0; AcX1=0; AcX2=0; AcX=0;
  }
  int i=0;
  while (i<5){
  if (knockValues[i] == passOne[i]){
    Serial.print(" Check:");
    Serial.print(i);
    i+=1;
  }
  else {
    Serial.println("");
    Serial.println("XXXXXXXXXXX DENIED XXXXXXXXXXX");;
    /*digitalWrite(servoBasePin, HIGH); // gives power to the servo
    myServo.write(20);*/
    delay(500); //change this back to 100 if using servo feedback
   /* myServo.write(40);
    delay(100);
    myServo.write(20);
    delay(300); // gives time for the servo to move back before cutting power
    digitalWrite(servoBasePin, LOW); //cuts power from servo*/
    // commented out code is for servo feedback if knock isn't correct.
    break;
  }
}
if (i==5){
  Serial.println("");
  Serial.println("---------- Door Open ----------");
  digitalWrite(servoBasePin, HIGH); // gives power to the servo
  myServo.write(180);
  delay(3000);
  myServo.write(20);
  delay(1000);
  digitalWrite(servoBasePin, LOW); // cuts power from servo
  Serial.println("Waiting For Knock");
  }
}
}
