#include <TinyWireM.h>
#include <USI_TWI_Master.h>

#include <tiny_IRremote.h>
#include <tiny_IRremoteInt.h>

/* Unfortunately, the Tiny has to be the master, as the
 * Tiny I2C library doesn't support onRequest, or onReceive,
 * both of which are essential... */


#define TRANSMIT_PIN 4
#define RECEIVE_PIN 3
#define CONTROL_PIN 1
#define MCU_ADDRESS 127

/*
10k pullups on CLK, DATA, CONTROL
transmitter LED is on pin 3 of chip
receiver is on pin 2 of chip
control line is on pin 6 of chip

SDA is pin 5
SCL is pin 7

control line on Arduino Micro is D12
*/

IRrecv irrecv(RECEIVE_PIN);

decode_results receiveValue;
IRsend irsend;
int padding = 0xF00;

int transmitValue = 0;

void setup() {
  pinMode(TRANSMIT_PIN, OUTPUT);

  TinyWireM.begin(); // begin i2c
  irrecv.enableIRIn(); // start receiver
}

void loop() {
  if (irrecv.decode(&receiveValue)) {
    
    TinyWireM.beginTransmission(MCU_ADDRESS); // initiate i2c transmission to mcu
    TinyWireM.send(byte(receiveValue.value - padding)); // subtract padding, constrain to 8 bits & send
    TinyWireM.endTransmission();
    
    irrecv.resume(); // Receive the next value
  }  

  // triggered by LOW on control pin
  if (!digitalRead(CONTROL_PIN)){
    
    TinyWireM.requestFrom(MCU_ADDRESS, 1); // request a byte from mcu
    transmitValue = TinyWireM.receive() + padding; // receive byte & add padding (4 leading 1s)
    
    irsend.sendSony(transmitValue, 12); // transmit padded byte as Sony-IR
    //delayMicroseconds(5000);
    
    irrecv.enableIRIn(); // restart receiver
  }
}

