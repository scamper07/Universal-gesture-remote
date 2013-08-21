#include <IRremote.h> // IR remote library

// Motor pins
int motorVar=8;
int motorFix=7;

// H-bridge enable pin
int enable = 2;

// IR receiver pin
int RECV_PIN = 11;

boolean ctrl=false;
IRrecv irrecv(RECV_PIN);

int state=0;
decode_results results;
int speeed=500; // initial speed of motor
void setup()
{
  pinMode(7,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(motorVar,OUTPUT);
  Serial.begin(9600);
  digitalWrite(7,LOW);
  digitalWrite(enable,HIGH);
  irrecv.enableIRIn(); // Start the receiver
}

void loop()
{ 
 
  if (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    if(results.value==2707)
    {
      state=1;
    }
      
    else if(results.value==2704)
    {
      state=2;
    }
      
    
    else if(results.value==1168 && ctrl==true && speeed<=900)
    {
      state=3;
      speeed+=10; // Increase speed of motor
    }
      
    else if(results.value==3216 && ctrl==true && speeed>=600)
    {
      state=4;
      speeed-=10; // Decrease speed of motor
    }
      
    
   
    irrecv.resume(); // Receive the next value
  }
  
  if(state==1)
  {
    digitalWrite(motorVar, HIGH);
    delayMicroseconds(speeed); 
    digitalWrite(motorVar, LOW);
    delayMicroseconds(1000 - speeed);

    
    ctrl=true;
  }

  else if(state==2)
  {
    digitalWrite(motorVar,LOW);
    ctrl=false;
    speeed=500;
  }  
   
   else if(state==3)
  {
    digitalWrite(motorVar, HIGH);
    delayMicroseconds(speeed); 
    digitalWrite(motorVar, LOW);
    delayMicroseconds(1000 - speeed);

  }
  
  else if(state==4)
  {
    digitalWrite(motorVar, HIGH);
    delayMicroseconds(speeed); 
    digitalWrite(motorVar, LOW);
    delayMicroseconds(1000 - speeed);
  }
    
    
}
