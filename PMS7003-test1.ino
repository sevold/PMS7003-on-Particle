//******************************
 //*Abstract: Read value of PM1,PM2.5 and PM10 of air quality
 //
 //*Version：V3.1
 //*Author：Zuyang @ HUST
 //*Modified by Cain for Arduino Hardware Serial port compatibility
 //*Date：March.25.2016
 //*References :
 //*    Thread : https://community.particle.io/t/dust-sensor-pms-5003-6003-7003/24221/
 //******************************
#include "Particle.h"

char buf[31];
int  bufIndex = -1;  // current index in message buffer (-1 indicates not started)

int PM01Value=0;          //define PM1.0 value of the air detector module
int PM2_5Value=0;         //define PM2.5 value of the air detector module
int PM10Value=0;         //define PM10 value of the air detector module

 
void setup()
{
  static unsigned long OledTimer=millis(); 
  Serial.begin(57600);
  Serial1.begin(9600);   //use serial0
  //Serial1.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
 
}

void SerialEvent1()
{
    int b = 0;
    
    // when not already started a message, flush everything up to first 'B'
    while (bufIndex < 0 && (b = Serial1.read()) >= 0 && b != 'B') Particle.process();  

    // if we got a start byte start a new message
    if ('B' == b) bufIndex = 0;  // had en error here, hence the following discussion (if (`B` = b) caused the problem)  

    // fill all available bytes into open message buffer
    while (0 <= bufIndex && bufIndex < sizeof(buf) && Serial1.available())
    {
        buf[bufIndex++] = Serial1.read();
        Particle.process();
    }
    
    if (bufIndex == sizeof(buf))
    {
        if(buf[0] == 0x4d && checkValue(buf,LENG)) 
        {
            PM01Value=transmitPM01(buf); //count PM1.0 value of the air detector module
            PM2_5Value=transmitPM2_5(buf);//count PM2.5 value of the air detector module
            PM10Value=transmitPM10(buf); //count PM10 value of the air detector module 
        }
        bufIndex = -1;  // mark as treated and ready for next message
    }
}

void loop()
{
  
    if (millis() - OledTimer >=1000) 
    {
      OledTimer=millis(); 

    Serial.printlnf("PM  1.0: %5d µg/m³\r\n"
                  "PM  2.5: %5d µg/m³\r\n"
                  "PM 10.0: %5d µg/m³\r\n"
                  ,PM01Value
                  ,PM2_5Value
                  ,PM10Value);
       
    }
   
}


char checkValue(char *thebuf, char leng)
{  
  char receiveflag=0;
  int receiveSum=0;
 
  for(int i=0; i<(leng-2); i++){
  receiveSum=receiveSum+thebuf[i];
  }
  receiveSum=receiveSum + 0x42;
  
  if(receiveSum == ((thebuf[leng-2]<<8)+thebuf[leng-1]))  //check the serial data 
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}
 
int transmitPM01(char *thebuf)
{
  int PM01Val;
  PM01Val=((thebuf[3]<<8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}
 
//transmit PM Value to PC
int transmitPM2_5(char *thebuf)
{
  int PM2_5Val;
  PM2_5Val=((thebuf[5]<<8) + thebuf[6]);//count PM2.5 value of the air detector module
  return PM2_5Val;
  }
 
//transmit PM Value to PC
int transmitPM10(char *thebuf)
{
  int PM10Val;
  PM10Val=((thebuf[7]<<8) + thebuf[8]); //count PM10 value of the air detector module  
  return PM10Val;
}
