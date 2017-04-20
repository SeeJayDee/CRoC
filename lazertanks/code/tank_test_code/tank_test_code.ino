
/*
 * turret pin 5
 * barrel pin 4
 */

 //TODO audio
 //TODO drive moters
 //TODO led
 //TODO ir


void ledColour(uint8_t, uint8_t, uint8_t);
void moveTurret(int);
void moveBarrel(int);
void motorControl(int, int);
void initializeTimers();

byte foo = 0;
void setup() {
  initializeTimers();
}

void loop() {
  moveTurret(analogRead(0)-512);
  moveBarrel(analogRead(1));
  //delay(15);
  //OCR3B = 4*foo + 1024;
  //foo++;
  delay(25);
} 

//#define LEFTM_INVERTED 1
//#define RIGHTM_INVERTED 1
// uncomment the relevant #define to invert the motor phase

#define LEFTM_PHASE_PIN 7
#define RIGHTM_PHASE_PIN 8

#define TURRET_ZERO_VALUE 2900
// turret zero value is the OCR3A value for which no rotation occurs. Upper and lower limits will be this value +-1000
// if it is difficult to make it stay still, we could shut off the pin when it is stationary.

#define BARREL_MINVAL 2000
#define BARREL_MAXVAL 4000
#define BARREL_NEUTRALPOS 3000
#define BARREL_JOYSTICK_NEUTRAL_VAL 512
#define BARREL_CONTROL_DEADBAND 16
//#define BARREL_INVERT_DIR 1
// we will need to determine the OCR3B values at the highest and lowest allowable elevations for the barrel.

//#define TURRET_PIN 5
//#define BARREL_PIN 4
// barrel servo is interrupt-driven
// what does 'turning off' the servo control signal (pin) do?

// functions: ledColour, moveTurret, moveBarrel, motors

void ledColour(uint8_t rDuty, uint8_t gDuty, uint8_t bDuty)
{
  OCR1A = rDuty;
  OCR1B = gDuty;
  OCR1C = bDuty;
}

void moveTurret(int tSpeed)
{
  tSpeed = constrain(tSpeed, -1000, 1000);
  OCR3A = TURRET_ZERO_VALUE + tSpeed;
}

void moveBarrel(int bInput)
{
  static int bPosition = BARREL_NEUTRALPOS;
  int change = map(bInput, 0, 1023, -100, 100);
  if ((change > 10) || (change < -10))
  {
    bPosition += change;
    bPosition = constrain(bPosition, BARREL_MINVAL, BARREL_MAXVAL);
  }
  OCR3B = bPosition;
}

/*void moveBarrel(int bInputVal)
{
  static byte bCounter = 0;
  static int b_OCRVal = BARREL_NEUTRALPOS;
  
#ifdef BARREL_INVERT_DIR
  byte i = -1;
#else
  byte i = 1;
#endif


  // if input from joystick is greater than (centre position) + (threshold)
  // RAISE the barrel
  if (bInputVal > BARREL_JOYSTICK_NEUTRAL_VAL + BARREL_CONTROL_DEADBAND)
  {
    // the servo pulse length increases by 0.5us every time bCounter is greater than 
    // a value determined by the joystick input. As the joystick is pushed further, 
    // bCounter doesn't have to increment as far.
    if (bCounter > 1023 - bInputVal)
    {
      bCounter = 0;
      b_OCRVal += i;
    }
    else bCounter++; // increase increment size if movement is too slow
  }
  else if (bInputVal < BARREL_JOYSTICK_NEUTRAL_VAL - BARREL_CONTROL_DEADBAND) // LOWER the barrel
  {
    // servo pulse length _DE_creases by 0.5us each time instead.
    if (bCounter > bInputVal)
    {
      bCounter = 0;
      b_OCRVal -= i;
    }
    else bCounter++;
  }
  else // joystick is too close to the centre
  {
    bCounter = 0;
  }
  
  OCR3B = constrain(b_OCRVal, BARREL_MINVAL, BARREL_MAXVAL);

}/**/

// so... inputs from -1023 to 1023 (just 'cause)
void motorControl(int l_Speed, int r_Speed)
{
#ifdef LEFTM_INVERTED
  l_Speed = -l_Speed;
#endif
#ifdef RIGHTM_INVERTED
  r_Speed = -r_Speed;
#endif

  if (l_Speed < 0)
  {
    digitalWrite(LEFTM_PHASE_PIN, LOW);
    l_Speed = -l_Speed;
  }
  else digitalWrite(LEFTM_PHASE_PIN, HIGH);
  
  OCR4A = write_10bit_register(constrain(l_Speed,0,1023));
  
  if (r_Speed < 0)
  {
    digitalWrite(RIGHTM_PHASE_PIN, LOW);
    r_Speed = -r_Speed;
  }
  else digitalWrite(RIGHTM_PHASE_PIN, HIGH);
  
  OCR4D = write_10bit_register(constrain(r_Speed,0,1023));
}

/* This code is to set up the timers and pin directions for all PWM pins on the Arduino Micro.
 * The output compare registers should really only be accessed from within other functions that 
 * provide better abstraction, eg "void moveBarrel(int angle)" rather than writing the raw pulse length
 * directly to OCR3B.
 */

/* Timer assignments:
 * Timer 1 - PWM mode (8-bit) - RGB leds           --- expect 7.81 kHz
 * Timer 3 - Interrupt mode, interrupts control servo pins  --- expect   50  Hz
 * Timer 4 - PWM mode (OC4A & OC4D)             --- expect 23.4 kHz
 */
 
/* PIN ASSIGNMENTS - Arduino Pin Number - (access register) [range]
 * Motor 1: D13 (OCR4A) [0-1023]; LEFT/A; 'Phase' pin is D7
 * Motor 2: D6  (OCR4D) [0-1023]; RIGHT/B; 'Phase' pin is D8
 * Servo 1: D5 (OCR3A) [0-39999]; half-microSeconds
 * Servo 2: D4 (OCR3B) [0-39999]; half-microSeconds; ***software (interrupt) -based
 * RGB Red: D9  (OCR1A) [0-255]
 * RGB Grn: D10 (OCR1B) [0-255]
 * RGB Blu: D11 (OCR1C) [0-255]
 *
 * If possible we should use something like:
 * #define MOTOR1_SPEED OCR4A
 * 
 * IMPORTANT: OCR4A and OCR4D can ONLY be written with a call to write_10bit_register()
 */
 
 void initializeTimers()
 {
  /* Rev 1 - code adjusted for unscaled system clock (16MHz) */
   
  /* enable global interrupts */
  sei();
     
   
  /* Initialize Timer 1 for 8-bit PWM */
  OCR1A = 0x0080; /* initialize all OCRs to 50% duty cycle (0x0080) */
  OCR1B = 0x0080;
  OCR1C = 0x0080;
   
  TCCR1A = 0x00; /* clear all bits */   
  TCCR1B = 0x00; /* clear all bits */
  
  TCCR1A |= (1<<COM1A1) + (1<<COM1B1) + (1<<COM1C1) + (1<<WGM10); /* clear on compare match; 8-bitFPWM */
    DDRB |= _BV(5) + _BV(6) + _BV(7); /* set dig. pins 9,10,11 (PD5,PD6,PD7) to output mode */

  TCCR1B |= (1<<WGM12) + (1<<CS11); /* 8-bitFastPWM; prescaler = 8 (expected freq = 7.8kHz) */
  /* End Timer 1 */ 
   
   
  /* Initialize Timer 3 for a 20ms period, with direct servo control output on OC3A and indirect servo
   * control via interrupt generated by OCR3B compare match */
  OCR3A = 0x00; /* IMPORTANT - initialize OCRs to 0 to prevent weirdness */
  OCR3B = 0x00;
  ICR3 = 39999; /* this sets the counter period to 20ms */
   
  TCCR3A = 0x00; /* clear all bits */   
  TCCR3B = 0x00; /* clear all bits */   
  TCCR3C = 0x00; /* clear all bits */   
  TIMSK3 = 0x00; /* clear all bits */
   
  TCCR3A |= (1<<COM3A1) + (1<<WGM31); /* clears OC3A (dig. pin 5) on compare match */
    DDRC |= _BV(6); /* Set dig. pin 5 (PC6) to output mode */

  TCCR3B |= (1<<WGM33) + (1<<WGM32) + (1<<CS31); /* FPWM mode with period defined by ICR3; prescaler = 8 */

  TIMSK3 |= (1<<ICIE3) + (1<<OCIE3B); /* enable interrupts for count==ICR3 and count==OCR3B */
    DDRD |= _BV(4); /* Set dig. pin 4 (PD4) to output mode*/
  /* End Timer 3 */
   
   
  /* Initialize Hi-speed Timer 4 for 10-bit PWM on OC4A (Dig. pin 13) & OC4D (Dig. pin 6) */
  OCR4C = write_10bit_register(1023); /* OCR4C defines the "TOP" value for Timer/Counter 4 */
  OCR4A = write_10bit_register(512);
  OCR4D = write_10bit_register(512);

  TCCR4A = 0x00; /* clear all bits */
  TCCR4B = 0x00; /* clear all bits */
  TCCR4C = 0x00; /* clear all bits */
  TCCR4D = 0x00; /* clear all bits */
  TCCR4E = 0x00; /* clear all bits */
  TIMSK4 = 0x00; /* clear all bits */

  TCCR4A |= (1<<COM4A1) + (1<<PWM4A); /* clear OC4A (Dig. pin 13) on compare match; enable OC4A PWM */
    DDRC |= _BV(7); /* Set dig. pin 13 (PC7) to output mode*/     

  TCCR4B |= (1<<CS40); /* prescaler = 1 */
   
  TCCR4C |= (1<<COM4D1) + (1<<PWM4D); /* clear OC4D (Dig. pin 6) on compare match; enable OC4D PWM */
    DDRD |= _BV(7); /* Set dig. pin 6 (PD7) to output mode*/

  // set up phase pins for motors (pins 7 and 8)
  DDRB |= _BV(4);
  DDRE |= _BV(6);
   
  /* PLL control --- This may or may not affect USB functionality... */
  /* Connecting or disconnecting USB might screw up Timer4 operation until the next reset.
   * If USB is connected and enabled, the PLL will need no further configuration. Otherwise it will need
   * to be set up as follows: */
  if ((USBCON & (1<<USBE)) == 0)
  {
    /* if USB is disabled (i.e. USBE bit in USBCON is 0), enable the PLL */
    PLLFRQ = 0x00;  /* clear all bits */
      PLLFRQ |= (1<<PLLTM1) + (1<<PDIV2);
      /* divides frequency sent to Timer4 by 1.5; Sets raw frequency to 48MHz */
     
    /* CODE COPIED FROM "USBCore.cpp" */
    PLLCSR |= (1<<PINDIV); /* divide 16MHz cpu clock by 2 for input into PLL */
    PLLCSR |= (1<<PLLE); /* enable PhaseLockedLoop */
    while (!(PLLCSR & (1<<PLOCK)))    // wait for lock pll
    {
    }

    /* Some tests on specific versions of macosx (10.7.3), reported some
    // strange behaviors when the board is reset using the serial
    // port touch at 1200 bps. This delay fixes this behavior. */
    delay(1);
    /* END COPIED CODE */    
  }
  /* End Timer 4 */

 /* End Timer Initializations */   
 }
 
ISR(TIMER3_CAPT_vect)
 {
  /* FOR SERVO
   * This interrupt is triggered when Timer3 reaches the value in ICR3 (39,999 in this case).
   * We set the output pin (Dig. pin 4) high at the end (aka beginning) of each timer cycle. */
  PORTD |= _BV(4); /* Set Dig. pin 4 (PD4) HIGH */
 }
 
ISR(TIMER3_COMPB_vect)
 {
  /* FOR SERVO
   * This interrupt is triggered when Timer3 reaches the value in OCR3B.
   * This time we clear the output pin (Dig. pin 4) at the end of each pulse. */
  PORTD &= ~(_BV(4)); /* Set Dig. pin 4 (PD4) LOW */
 }
 

 
uint8_t write_10bit_register(uint16_t i)
 {
  TC4H = (i >> 8);
  return (uint8_t)(i & 0xff);
 }
 
