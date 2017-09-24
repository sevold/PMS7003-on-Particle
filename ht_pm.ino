// This #include statement was automatically added by the Particle IDE.
#include <HttpClient.h>

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h>

//Abstract: Dweeting data from sensors for humidity, temperature, and particulate matter
//Sensors: DHT22, PMS7003
//Sources: code from ladyada and ilak2k


#include <Particle.h>
#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
char buf[LENG];

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11		// DHT 11 
#define DHTTYPE DHT22		// DHT 22 (AM2302)
//#define DHTTYPE DHT21		// DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);
 
int PM01Value=0;          //define PM1.0 value of the air detector module
int PM2_5Value=0;         //define PM2.5 value of the air detector module
int PM10Value=0;         //define PM10 value of the air detector module


unsigned int nextTime = 0;    // Next time to contact the server
HttpClient http;

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers with NULL
};

http_request_t request;
http_response_t response;

 
void setup()
{
  Serial1.begin(9600);   //use serial1 for PMS7003
  dht.begin();    //initiate DHT sensor
}
 
void loop()
{
    //delay(1000)
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
	
	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a 
	// very slow sensor)
	float h = dht.getHumidity();
	// Read temperature as Celsius
	float t = dht.getTempCelcius();
	// Read temperature as Farenheit
	float f = dht.getTempFarenheit();
  
	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f)) {
		Particle.publish("DEBUG","Failed to read from DHT sensor!");
		return;
	}

	// Compute heat index
	// Must send in temp in Fahrenheit!
	float hi = dht.getHeatIndex();
	float dp = dht.getDewPoint();
	float k = dht.getTempKelvin();
	
	//cast floats to string
	String st(t, 2);
	String sh(h, 2);

	//post to dweet
	request.hostname = "dweet.io";
	request.port = 80;
	request.path = "/dweet/for/brenchieslabdata?pm1=" + pm01 + "&pm2_5=" + pm2_5 + "&pm10=" + pm10 + "&temp=" + st + "&humidity=" + sh;

	Particle.publish("DEBUG",request.body);
	    
	// Get request
	http.get(request, response, headers);

	//Particle.publish("DEBUG",response.status);
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
