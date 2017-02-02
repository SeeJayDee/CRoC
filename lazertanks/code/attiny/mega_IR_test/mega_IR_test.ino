#include <Wire.h>

#define IR_ADDR 0xA
#define ACK_CODE 0x80

#define CLK_PIN 23
#define DATA_PIN 22
#define CTRL_PIN 18
#define CTRL2_PIN 12
#define DEBUG_PIN 10

#define ID_LEN 12
#define TRANSMIT_ID 0xAA

volatile bool receivedFlag = false;
uint8_t received;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  pinMode(CTRL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CTRL_PIN), getIR_Value, FALLING);
  pinMode(DEBUG_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    if (Serial.read() == '\n'){
      pinMode(CTRL2_PIN, OUTPUT);
      digitalWrite(CTRL2_PIN, LOW);
      delayMicroseconds(100);
      digitalWrite(CTRL2_PIN, HIGH);
      pinMode(CTRL2_PIN, INPUT);
      //Wire.beginTransmission(IR_ADDR);
      //Wire.write(TRANSMIT_ID);
      //Wire.endTransmission();
    }
  //digitalWrite(DEBUG_PIN, LOW);

  }
  if(receivedFlag){
    /*Wire.beginTransmission(IR_ADDR);
    Wire.write(ACK_CODE);
    Wire.endTransmission();*/
    Wire.requestFrom(IR_ADDR, 1, true);
    delayMicroseconds(100);
    received = Wire.read();
    Serial.println(received, HEX);
    received = 0;
    receivedFlag = false;
    //attachInterrupt(digitalPinToInterrupt(CTRL_PIN), getIR_Value, FALLING);
  }
  /*if (digitalRead(CTRL_PIN) == LOW){
    Wire.beginTransmission(IR_ADDR);
    Wire.write(ACK_CODE);
    Wire.endTransmission();
    Wire.requestFrom(IR_ADDR, 1, true);
    received = Wire.read();
    pinMode(CLK_PIN, OUTPUT);
    digitalWrite(CLK_PIN, LOW);
    received = int(shiftIn(DATA_PIN, CLK_PIN, LSBFIRST)) << 8;
    received += shiftIn(DATA_PIN, CLK_PIN, LSBFIRST);
    received >>= 4;
    pinMode(CLK_PIN, INPUT);
    Serial.println(received, HEX);
  }*/
}

void getIR_Value(){
  receivedFlag = true;
  /*detachInterrupt(digitalPinToInterrupt(CTRL_PIN));
  Wire.requestFrom(IR_ADDR, 1, true);
  digitalWrite(DEBUG_PIN, HIGH);
  //delayMicroseconds(125);
  while(digitalRead(CTRL_PIN) == LOW) {}
  
  received = Wire.read();*/
}


