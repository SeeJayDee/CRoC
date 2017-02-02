#include<Wire.h>

#define MCU_ADDR 127
#define CTRL_PIN 12
#define LED1 6
#define LED2 7

char outChar = 0, inChar = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);

  Wire.begin(MCU_ADDR);
  Wire.onRequest(transmitByte);
  Wire.onReceive(receiveByte);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    outChar = Serial.read(); // get char from serial
    Serial.print("Sending: ");
    Serial.println(outChar); // echo value back
    pinMode(CTRL_PIN, OUTPUT);
    digitalWrite(CTRL_PIN, LOW); // assert control line low
  }

  // switch the LEDs on or off depending on inChar
  // LEDs are active-low
  if (isLowerCase(inChar)){
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,LOW);
  }
  else if (isUpperCase(inChar)){
    digitalWrite(LED2,HIGH);
    digitalWrite(LED1,LOW);
  }
  else {
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
  }
}

// onRequest handler
void transmitByte(){
  Wire.write(outChar); // send input character to Tiny
  digitalWrite(CTRL_PIN, HIGH); // pull control line high
  pinMode(CTRL_PIN, INPUT); // hi-impedance
}

// onReceive handler
void receiveByte(int numBytes){
  while(!Wire.available()){
    // wait until data begins to arrive
  }
  inChar = Wire.read(); // read received byte into inChar
  
  Serial.print("Received: \""); // print as char and as hex
  Serial.print(inChar);
  Serial.print("\". Hex: ");
  Serial.println(inChar, HEX);
}

