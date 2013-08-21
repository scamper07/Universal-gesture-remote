#include <IRremote.h> // IR remote library
int led=9;
int RECV_PIN = 11; // Receiver pin

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  pinMode(9,OUTPUT);
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    if(results.value==2707)
    {
      digitalWrite(led,LOW); //switch light off
    }
    else if(results.value==2704)
    {
      digitalWrite(led,HIGH); //switch light on
    }
      irrecv.resume(); // Receive the next value
  }
}
