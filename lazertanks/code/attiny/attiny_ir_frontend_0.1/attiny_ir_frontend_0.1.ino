/*consider making the tiny a MASTER because
the attiny slave library doesn't yet support onRequest */

#include <tiny_IRremote.h>
#include <tiny_IRremoteInt.h>

#include <TinyWireS.h>
#include <usiTwiSlave.h>

#define TRANSMIT_PIN 4
#define RECEIVE_PIN 3
#define CONTROL_PIN 1
#define SLAVE_ADDRESS 127

/*
10k pullups on CLK, DATA, CONTROL
transmitter LED is on pin 3 of chip
receiver is on pin 2 of chip
control line is on pin 6 of chip
*/

IRrecv irrecv(RECEIVE_PIN);

decode_results results;
IRsend irsend;

uint8_t transmitValue = 0;
uint8_t receiveValue = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(TRANSMIT_PIN, OUTPUT);

  TinyWireS.begin(SLAVE_ADDRESS); // begin i2c
  irrecv.enableIRIn(); // start receiver
}

void loop() {
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results)) {
    sendResults();
    irrecv.resume(); // Receive the next value
  }  

  if (TinyWireS.available()){
    transmitValue = TinyWireS.receive();
    irsend.sendSony(transmitValue, 8);
    irrecv.resume();
  }
}

void sendResults(){
  /* pulse control pin to tell master there is data
     wait for request
     send data

     ?? is there a way to have the data ready-to-go, and transmit it UPON REQUEST??
     look into this...
  */
}
}

