#include <IRremote.h>                      //IR remote library

IRsend irsend;

int val = 0;                               // value read by force sensor 
int force = A1;                            // force sensor connected to analog pin #1

// control variables
boolean control = false;
int state = 0;

int reference = 0;                         // Stores the reference value after calibrating the wrist band to the user's hand
int sum=0;

int led=13;                                // indicator led

// accelerometer data
float acclReference = 0;
float acclNew = 0;

/* 
 
 Accelerometer hardware setup:
 MMA8452 Breakout ------------ Arduino
 3.3V --------------------- 3.3V
 SDA ----------------------- A4
 SCL ----------------------- A5
 INT2 ---------------------- D3
 INT1 ---------------------- D2
 GND ---------------------- GND
 
 SDA and SCL should have external pull-up (10k) resistors (to 3.3V).
  
 
 Note: The MMA8452 is an I2C sensor, however this code does
 not make use of the Arduino Wire library. Because the sensor
 is not 5V tolerant, we can't use the internal pull-ups used
 by the Wire library. Instead use the i2c.h, defs.h and types.h files.
 */
 
#include "i2c.h"  

#define SA0 1
#if SA0
#define MMA8452_ADDRESS 0x1D               //SA0 is high, 0x1C if low
#else
#define MMA8452_ADDRESS 0x1C
#endif

// Set the scale below either 2, 4 or 8
const byte SCALE = 2;  // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.
// Set the output data rate below. Value should be between 0 and 7
const byte dataRate = 0;  // 0=800Hz, 1=400, 2=200, 3=100, 4=50, 5=12.5, 6=6.25, 7=1.56

// Pin definitions
int int1Pin = 2;                           // These can be changed, 2 and 3 are the Arduinos ext int pins
int int2Pin = 3;  

int accelCount[3];                         // Stores the 12-bit signed value
float accelG[3];                           // Stores the real accel value in g's

int acclNew_z=0;
int acclState=0;
int acclNew_y=0;


void setup()
{
  Serial.begin(57600);                     // Serial communication initialization
  
  pinMode(force,INPUT);
  
  // force sensor calibration
  for (int i=0;i<20;i++)
  {
    val=analogRead(force);
    sum+=val;
  }
  reference=sum/20;
  Serial.println(" Calibration done ... ");
  
  //indicator led
  pinMode(led,OUTPUT);
  
  //Accelerometer
  byte c;

  // Set up the interrupt pins, they're set as active high, push-pull
  pinMode(int1Pin, INPUT);
  digitalWrite(int1Pin, LOW);
  pinMode(int2Pin, INPUT);
  digitalWrite(int2Pin, LOW);
  
  

  // Read the WHO_AM_I register, this is a good test of communication
  c = readRegister(0x0D);  // Read WHO_AM_I register
  if (c == 0x2A) // WHO_AM_I should always be 0x2A
  {  
    initMMA8452(SCALE, dataRate);  // init the accelerometer if communication is OK
    Serial.println("MMA8452Q is online...");
  }
  else
  {
    Serial.print("Could not connect to MMA8452Q: 0x");
    Serial.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }
    
}

void loop()
{
   static byte source;
   
   if (digitalRead(int1Pin)==1)  // Interrupt pin, should probably attach to interrupt function
   {
    readAccelData(accelCount);
    for (int i=0; i<3; i++)
      accelG[i] = (float) accelCount[i]/((1<<12)/(2*SCALE));
    acclNew = accelG[1]+1.0;
   }
   
   // gesture recognition by force sensor
  
   while(acclNew >= 0.6 && acclNew <= 1.3)
   {
     val = analogRead(force); 
     reference = 0.6*reference + 0.4*val;
     if(val >(reference-45) && val < (reference+45) && (state == 0 ||state== 2 ||state== 3))
     {
       Serial.println("Hand in normal position");
       digitalWrite(led,LOW);
       if(state == 2 && control == false)
       {
         for(int i=0; i<10; i++)
         {
           irsend.sendSony(0xa93,12);          //switch off device
           delay(40);
         }
         Serial.println("code a93 sent");
       } 
       state = 1;
     }
 
  if(val<(reference-90) && (state == 0 ||state== 1 || state==3))
  {
    Serial.println("Hand is closed");
    digitalWrite(led,HIGH);
    if (digitalRead(int1Pin)==1)               // Interrupt pin, should probably attach to interrupt function
    {
      readAccelData(accelCount);
      for (int i=0; i<3; i++)
        accelG[i] = (float) accelCount[i]/((1<<12)/(2*SCALE));
      acclReference = accelG[1] + 1.0;
    
    }
    state = 2;
  }
  
  if(val>(reference+95) && (state == 0 || state==1 || state==2))
  {
    Serial.println("Hand is open");
    digitalWrite(led,LOW);
    if(state==1)
    {
      for(int i=0; i<3; i++)
      {  
        irsend.sendSony(0xa90,12);              // switch on device
        delay(40);
      }
      Serial.println("code a90 sent");
    }
  state = 3;
  }
  
  if (digitalRead(int1Pin)==1) 
  {
    readAccelData(accelCount);
    for (int i=0; i<3; i++)
      accelG[i] = (float) accelCount[i]/((1<<12)/(2*SCALE));
    acclNew = accelG[1]+1.0;
  }  
  }
  control = false;
  
  // Swipe gesture recognition
  
  while(accelCount[1]<-700 && state != 2)
  {
      readAccelData(accelCount);
      acclNew_z=accelCount[2]+1000;
        
      if ( acclNew_z < 850) 
      {
        if(acclState==0)
        {
          acclState=2;
        }
        
        else if(acclState==1)
        {
          acclState=12;
        }
        control=true;
        
      }
      if(acclNew_z >1600 )
      {
        if(acclState==0)
        {
          acclState=1;
        }
        else if(acclState==2)
        {
          acclState=21;
        }
      }
      
      if(acclState==21)
      {
        Serial.println("Next");
        acclState=0;
        delay(150);
        irsend.sendRC6(0xc00020,24);
        delay(750);
      }
      if(acclState==12)
      {
        Serial.println("Previous");
        acclState=0;
        delay(50);
        irsend.sendRC6(0xc00021,24);
        delay(750);
      }
     //delay(100);   
    }
    
  readAccelData(accelCount);  
  
  // wrist rotation detection by accelerometer
    
  while(state==2 && accelCount[1]>-600 && accelCount[1]<-400)
  {
      digitalWrite(led,LOW);
      for(int i=0; i<3; i++)
      {  
      irsend.sendSony(0x490,12);
      delay(40);
      }
      digitalWrite(led,HIGH);
      
      Serial.println(" increase speed");
      control=true;
      
      val = analogRead(force);

      if(val >(reference-45) && val < (reference+45) && (state == 0 ||state== 2 ||state== 3))
      {
        Serial.println("Hand in normal position");
        digitalWrite(led,LOW);
        state = 1;
        break;
      }
      
      if(val>(reference+95) && (state == 0 || state==1 || state==2)) 
      {
        Serial.println("Hand is open");
        digitalWrite(led,LOW);
        state = 3;
        break;
      }
      
      readAccelData(accelCount);
      }
  
  while(state==2 && accelCount[1]>250 && accelCount[1]<450)
  {
      digitalWrite(led,LOW);
      for(int i=0; i<3; i++){
      irsend.sendSony(0xc90,12);
      delay(40);
      }
      digitalWrite(led,HIGH);
      
     Serial.println("decrease speed");
     control=true;
     readAccelData(accelCount);
  }

}

// Functions

void readAccelData(int * destination)
{
  byte rawData[6];  // x/y/z accel register data stored here

  readRegisters(0x01, 6, &rawData[0]);  // Read the six raw data registers into data array

  // Loop to calculate 12-bit ADC and g value for each axis
  for (int i=0; i<6; i+=2)
  {
    destination[i/2] = ((rawData[i] << 8) | rawData[i+1]) >> 4;  // Turn the MSB and LSB into a 12-bit value
    if (rawData[i] > 0x7F)
    {  
      // If the number is negative, we have to make it so manually (no 12-bit data type)
      destination[i/2] = ~destination[i/2] + 1;
      destination[i/2] *= -1;  // Transform into negative 2's complement #
    }
  }
}



void initMMA8452(byte fsr, byte dataRate)
{
  MMA8452Standby();  // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  if ((fsr==2)||(fsr==4)||(fsr==8))
    writeRegister(0x0E, fsr >> 2);  
  else
    writeRegister(0x0E, 0);

  // Setup the 3 data rate bits, from 0 to 7
  writeRegister(0x2A, readRegister(0x2A) & ~(0x38));
  if (dataRate <= 7)
    writeRegister(0x2A, readRegister(0x2A) | (dataRate << 3));  

  // Set up portrait/landscap registers - 4 steps:
  // 1. Enable P/L
  // 2. Set the back/front angle trigger points (z-lock)
  // 3. Set the threshold/hysteresis angle
  // 4. Set the debouce rate
  // For more info check out this app note: http://cache.freescale.com/files/sensors/doc/app_note/AN4068.pdf
  writeRegister(0x11, 0x40);  // 1. Enable P/L
  writeRegister(0x13, 0x44);  // 2. 29deg z-lock (don't think this register is actually writable)
  writeRegister(0x14, 0x84);  // 3. 45deg thresh, 14deg hyst (don't think this register is writable either)
  writeRegister(0x12, 0x50);  // 4. debounce counter at 100ms (at 800 hz)

  /* Set up single and double tap - 5 steps:
   1. Set up single and/or double tap detection on each axis individually.
   2. Set the threshold - minimum required acceleration to cause a tap.
   3. Set the time limit - the maximum time that a tap can be above the threshold
   4. Set the pulse latency - the minimum required time between one pulse and the next
   5. Set the second pulse window - maximum allowed time between end of latency and start of second pulse
   for more info check out this app note: http://cache.freescale.com/files/sensors/doc/app_note/AN4072.pdf */
  writeRegister(0x21, 0x7F);  // 1. enable single/double taps on all axes
  // writeRegister(0x21, 0x55);  // 1. single taps only on all axes
  // writeRegister(0x21, 0x6A);  // 1. double taps only on all axes
  writeRegister(0x23, 0x20);  // 2. x thresh at 2g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(0x24, 0x20);  // 2. y thresh at 2g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(0x25, 0x08);  // 2. z thresh at .5g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(0x26, 0x30);  // 3. 30ms time limit at 800Hz odr, this is very dependent on data rate, see the app note
  writeRegister(0x27, 0xA0);  // 4. 200ms (at 800Hz odr) between taps min, this also depends on the data rate
  writeRegister(0x28, 0xFF);  // 5. 318ms (max value) between taps max

  // Set up interrupt 1 and 2
  writeRegister(0x2C, 0x02);  // Active high, push-pull interrupts
  writeRegister(0x2D, 0x19);  // DRDY, P/L and tap ints enabled
  writeRegister(0x2E, 0x01);  // DRDY on INT1, P/L and taps on INT2

  MMA8452Active();  // Set to active to start reading
}

// Sets the MMA8452 to standby mode.
// It must be in standby to change most register settings
void MMA8452Standby()
{
  byte c = readRegister(0x2A);
  writeRegister(0x2A, c & ~(0x01));
}

// Sets the MMA8452 to active mode.
// Needs to be in this mode to output data
void MMA8452Active()
{
  byte c = readRegister(0x2A);
  writeRegister(0x2A, c | 0x01);
}

// Read i registers sequentially, starting at address into the dest byte array
void readRegisters(byte address, int i, byte * dest)
{
  i2cSendStart();
  i2cWaitForComplete();

  i2cSendByte((MMA8452_ADDRESS<<1)); // write 0xB4
  i2cWaitForComplete();

  i2cSendByte(address);	// write register address
  i2cWaitForComplete();

  i2cSendStart();
  i2cSendByte((MMA8452_ADDRESS<<1)|0x01); // write 0xB5
  i2cWaitForComplete();
  for (int j=0; j<i; j++)
  {
    i2cReceiveByte(TRUE);
    i2cWaitForComplete();
    dest[j] = i2cGetReceivedByte(); // Get MSB result
  }
  i2cWaitForComplete();
  i2cSendStop();

  cbi(TWCR, TWEN); // Disable TWI
  sbi(TWCR, TWEN); // Enable TWI
}

// Read a single byte from address and return it as a byte
byte readRegister(uint8_t address)
{
  byte data;

  i2cSendStart();
  i2cWaitForComplete();

  i2cSendByte((MMA8452_ADDRESS<<1)); // Write 0xB4
  i2cWaitForComplete();

  i2cSendByte(address);	// Write register address
  i2cWaitForComplete();

  i2cSendStart();

  i2cSendByte((MMA8452_ADDRESS<<1)|0x01); // Write 0xB5
  i2cWaitForComplete();
  i2cReceiveByte(TRUE);
  i2cWaitForComplete();

  data = i2cGetReceivedByte();	// Get MSB result
  i2cWaitForComplete();
  i2cSendStop();

  cbi(TWCR, TWEN);	// Disable TWI
  sbi(TWCR, TWEN);	// Enable TWI

  return data;
}

// Writes a single byte (data) into address
void writeRegister(unsigned char address, unsigned char data)
{
  i2cSendStart();
  i2cWaitForComplete();

  i2cSendByte((MMA8452_ADDRESS<<1)); // Write 0xB4
  i2cWaitForComplete();

  i2cSendByte(address);	// Write register address
  i2cWaitForComplete();

  i2cSendByte(data);
  i2cWaitForComplete();

  i2cSendStop();
}
