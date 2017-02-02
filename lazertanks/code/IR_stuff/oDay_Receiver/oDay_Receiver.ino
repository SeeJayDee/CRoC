#include<Wire.h>

#define MCU_ADDR 127
#define CTRL_PIN 12
#define SOUND_PIN 2
#define RESET_PIN 4
#define LED_PIN 5
#define IRCODE 0xAA

char outChar = 0xFF;
byte inChar = 0, counter = 0, ledDuty = 0;
unsigned long Time = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(SOUND_PIN, OUTPUT);
  digitalWrite(SOUND_PIN,HIGH);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN,HIGH);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,LOW);


  Wire.begin(MCU_ADDR);
  Wire.onRequest(transmitByte);
  Wire.onReceive(receiveByte);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 /* if (Serial.available()){
    outChar = Serial.read(); // get char from serial
    Serial.print("Sending: ");
    Serial.println(outChar); // echo value back
    pinMode(CTRL_PIN, OUTPUT);
    digitalWrite(CTRL_PIN, LOW); // assert control line low
  }*/

  // switch the LEDs on or off depending on inChar
  // LEDs are active-low
  if (Time < millis()){
    digitalWrite(SOUND_PIN, HIGH);
  }

  if (ledDuty){
    analogWrite(LED_PIN, ledDuty);
    if (counter) counter--;
    else {
      ledDuty--;
      counter = 255;
    }
  }
  else digitalWrite(LED_PIN, LOW);
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
  delay(10);
  Serial.println(inChar, HEX);
 /* Serial.print("Received: \""); // print as char and as hex
  Serial.print(inChar);
  Serial.print("\". Hex: ");*/

    if (inChar == IRCODE){
      Time = millis() + 100;
      digitalWrite(SOUND_PIN,LOW);
      inChar = 0;
      Serial.println("BOOM!");
      ledDuty = 255;
    }
  
}

