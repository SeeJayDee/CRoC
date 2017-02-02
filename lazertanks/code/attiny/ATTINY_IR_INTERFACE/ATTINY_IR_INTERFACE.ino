#include <TinyWireS.h>
#include <usiTwiSlave.h>

#include <tiny_IRremote.h>
#include <tiny_IRremoteInt.h>

#define SELF_ADDR 0xA
#define ACK_CODE 0x80

#define LED_PIN 4
#define CTRL_PIN 1
#define RECV_PIN 3
/* external 10k pullups on CLK, DATA, CTRL pins */
/* LED is on pin 4 (PB4; OC1B; dip-3)
   Receiver is on pin 3 (PB3; dip-2)
   CTRL line is pin 1 (PB1; dip-6) */

IRrecv irrecv(RECV_PIN);

decode_results results;
IRsend irsend;

const uint8_t transmitID = 0xAA;
#define ID_LEN 8

void setup(){
	pinMode(LED_PIN, OUTPUT);
	pinMode(CTRL_PIN, INPUT);
	pinMode(RECV_PIN, INPUT);

  TinyWireS.begin(SELF_ADDR);
	
	irrecv.enableIRIn(); // Start the receiver
}

void loop(){
  if (irrecv.decode(&results)) {
    sendResults();
    irrecv.resume(); // Receive the next value
  }	
  
  /*if (TinyWireS.available()){
	  irsend.sendSony(TinyWireS.receive(), ID_LEN);
    irrecv.resume();
  }*/
}

void sendResults(){
	pinMode(CTRL_PIN, OUTPUT);
	digitalWrite(CTRL_PIN, LOW); // assert CTRL_PIN low to initiate transfer request
  delayMicroseconds(10);  
  digitalWrite(CTRL_PIN, HIGH);
  delayMicroseconds(10);
  TinyWireS.send(byte(results.value));
	/*while(digitalRead(CLK_PIN)) {} // loop until CLK_PIN is asserted LOW (request acknowledged)
	
	for (byte i=0; i<ID_LEN; i++){
		while(digitalRead(CLK_PIN) == LOW) {}
    pinMode(DATA_PIN, OUTPUT);
		digitalWrite(DATA_PIN, (results.valuetransmitID)&_BV(i)>>i);
	}
	pinMode(CLK_PIN, OUTPUT);
	digitalWrite(CLK_PIN, LOW);
	
	pinMode(DATA_PIN, OUTPUT);
	shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, byte(results.value));
	
	pinMode(CLK_PIN, INPUT);
	pinMode(DATA_PIN, INPUT);*/
	pinMode(CTRL_PIN, INPUT);	
}
