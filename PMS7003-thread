
char buf[31];
int  bufIndex = -1;  // current index in message buffer (-1 indicates not started)

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
