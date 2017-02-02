#include<Wire.h>

#define MCU_ADDR 127
#define CTRL_PIN 12
#define LED1 6
#define LED2 7
#define BUTTON_PIN 10
#define BUTTON_PULLUP 13

char outChar = 0, inChar = 0, i=0;
bool isTransmitting = false;
unsigned long Time = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON_PULLUP, OUTPUT);
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,LOW);
  digitalWrite(BUTTON_PULLUP,HIGH);

  Wire.begin(MCU_ADDR);
  Wire.onRequest(transmitByte);
  Wire.onReceive(receiveByte);
  //Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(BUTTON_PIN)&&!isTransmitting){
    Time = millis() + 333;
    switch (i){
      case 0: outChar = 'a';
              break;
      case 1: outChar = 'A';
              break;
      case 2: outChar = '~';
              break;
    }
    i++;
    i %= 3;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    pinMode(CTRL_PIN, OUTPUT);
    digitalWrite(CTRL_PIN, LOW); // assert control line low
    isTransmitting = true;
  }

  if (isTransmitting&&(!digitalRead(BUTTON_PIN))&&(millis()>Time)){
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  isTransmitting = false;
  }

  // switch the LEDs on or off depending on inChar
  // LEDs are active-low

}

// onRequest handler
void transmitByte(){
  Wire.write(outChar); // send input character to Tiny
  digitalWrite(CTRL_PIN, HIGH); // pull control line high
  pinMode(CTRL_PIN, INPUT); // hi-impedance
  digitalWrite(LED2, HIGH);
}

// onReceive handler
void receiveByte(int numBytes){
  while(!Wire.available()){
    // wait until data begins to arrive
  }
  inChar = Wire.read(); // read received byte into inChar
  
 /* Serial.print("Received: \""); // print as char and as hex
  Serial.print(inChar);
  Serial.print("\". Hex: ");
  Serial.println(inChar, HEX);*/
}

