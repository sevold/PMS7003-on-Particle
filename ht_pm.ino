//Abstract: Dweeting data from sensors for humidity, temperature, and particulate matter
//Sensors: DHT22, PMS7003
//


#include <HttpClient.h>
#include <Particle.h>
#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
char buf[LENG];
 
int PM01Value=0;          //define PM1.0 value of the air detector module
int PM2_5Value=0;         //define PM2.5 value of the air detector module
int PM10Value=0;         //define PM10 value of the air detector module


unsigned int nextTime = 0;    // Next time to contact the server
HttpClient http;

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

http_request_t request;
http_response_t response;

 
void setup()
{
  //Serial.begin(9600);
  Serial1.begin(9600);   //use serial1
  //Serial1.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
 
}
 
void loop()
{
    //odelay(1000)
  if(Serial1.find("B")) {    //start to read when detect 0x42
    Serial1.readBytes(buf,LENG);
 
    if(buf[0] == 0x4d){
      if(checkValue(buf,LENG)){
        PM01Value=transmitPM01(buf); //count PM1.0 value of the air detector module
        PM2_5Value=transmitPM2_5(buf);//count PM2.5 value of the air detector module
        PM10Value=transmitPM10(buf); //count PM10 value of the air detector module 
      }           
    } 
  }
 
  static unsigned long OledTimer=millis();  
    if (millis() - OledTimer >=1000) 
    {
      OledTimer=millis();
      
      String pm01 = String(PM01Value);
      String pm2_5 = String(PM2_5Value);
      String pm10 = String(PM10Value);
      
      	
	//post to dweet
	request.hostname = "dweet.io";
    request.port = 80;
    request.path = "/dweet/for/brenchieslabdata?pm1=" + pm01 + "&pm2_5=" + pm2_5 + "&pm10=" + pm10;
    
    	
	Particle.publish("DEBUG",request.body);
	
    // Get request
    http.get(request, response, headers);
    //Spark.publish("DEBUG","Application Response status: ");
    //Particle.publish("DEBUG",response.status);

    //Spark.publish("DEBUG","Application HTTP Response Body: ");
    Particle.publish("DEBUG",response.body);
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
