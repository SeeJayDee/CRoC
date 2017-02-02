#include <SendOnlySoftwareSerial.h>

/*
  Serial Event example

 When new serial data arrives, this sketch adds it to a String.
 When a newline is received, the loop prints the string and
 clears it.

 A good test for this is to try it with a GPS receiver
 that sends out NMEA 0183 sentences.

 Created 9 May 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialEvent

 */


SendOnlySoftwareSerial MP3Player = SendOnlySoftwareSerial(2);
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(9600);
  MP3Player.begin(9600);
  MP3Player.write(0x0C);
  delayMicroseconds(30000);
  MP3Player.write(0x08);
  MP3Player.write(byte(0));
  MP3Player.write(2);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    if (inputString.length() == 1){
      MP3Player.write(0x16);
      Serial.println("1");
      MP3Player.write(0x03);
      MP3Player.write(byte(0));
      MP3Player.write(1);
      digitalWrite(13, LOW);
    }
    else if (inputString.length() == 2){
      MP3Player.write(0x16);
      Serial.println("2");
      MP3Player.write(0x03);
      MP3Player.write(byte(0));
      MP3Player.write(5);
      digitalWrite(13, LOW);
    }
    else if (inputString.length() == 3){
      MP3Player.write(0x16);
      Serial.println("3");
      MP3Player.write(0x03);
      MP3Player.write(byte(0));
      MP3Player.write(12);
      digitalWrite(13, LOW);
    }
    else {
      MP3Player.write(0x16);
      Serial.println("4");
      MP3Player.write(0x03);
      MP3Player.write(byte(0));
      MP3Player.write(13);
      digitalWrite(13, LOW);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


