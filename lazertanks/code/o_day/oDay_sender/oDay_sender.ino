#include <Servo.h>
#include <IRremote.h>
#include <IRremoteInt.h>

#define SERVO_PIN 7
#define TX_PIN 13
#define BTN_PIN 2
#define JOYSTK_PIN 5   //A5
#define SOUND_PIN 3

#define IRCODE 0xFFFFFFAA //the code which will be transmitted
#define SRVOFFSET 925  //add this to analogRead value to get microseconds

#define REVERSE 1

Servo Turret;
IRsend lazerGun;

int joyStickVal = 512;
unsigned long Time = 0;
bool isSending = false;

void setup() {
  Turret.attach(SERVO_PIN); // attach turret servo
  Turret.writeMicroseconds(1500); // zero rotation
  
  pinMode(SOUND_PIN, OUTPUT);
  digitalWrite(SOUND_PIN, HIGH); //active-low

  pinMode(BTN_PIN, INPUT_PULLUP); //activate internal pullups on button
  //Serial.begin(9600);
}

void loop() {
  joyStickVal = analogRead(JOYSTK_PIN);
  
  // if the button was pressed...
  if ((!digitalRead(BTN_PIN))&&(!isSending)){
    Time = millis() + 333;
    isSending = true; // set flag

    digitalWrite(SOUND_PIN, LOW); // trigger sound
    lazerGun.sendNEC(IRCODE, 32); // send IRCODE as NEC (38kHz - might be better)
  }

  // allow 1/3 sec to elapse before resetting sound trigger and send flag
  if (millis() > Time){
    digitalWrite(SOUND_PIN, HIGH); // prevent looping
    if(digitalRead(BTN_PIN)&&isSending) isSending = false;
  }

  #ifdef REVERSE
  joyStickVal = 1023 - joyStickVal;
  #endif
  
  Turret.writeMicroseconds(joyStickVal + SRVOFFSET);
  
  //Serial.println(joyStickVal);
}
