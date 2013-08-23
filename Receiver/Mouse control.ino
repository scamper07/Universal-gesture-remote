#include <IRremote.h>                   //IR remote library

int RECV_PIN = 11;                      //pin to which IR receiver is connected

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();                  // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    Serial.flush();
    delay(62);
    irrecv.resume();                    // Receive the next value
  }
  Serial.flush();                       // clear serial buffer
}
