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
    outChar = Serial.read();
    Serial.print("Sending: ");
    Serial.println(outChar);
    pinMode(CTRL_PIN, OUTPUT);
    digitalWrite(CTRL_PIN, LOW);
  }

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

void transmitByte(){
  Wire.write(outChar);
  digitalWrite(CTRL_PIN, HIGH);
  pinMode(CTRL_PIN, INPUT);
}

void receiveByte(int numBytes){
  while(!Wire.available()){
    
  }
  inChar = Wire.read();
  
  Serial.print("Received: \"");
  Serial.print(inChar);
  Serial.print("\". Hex: ");
  Serial.println(inChar, HEX);
}

