/* Timer assignments:
 * Timer 0 - CTC mode - IR transmitter 						--- expect 38.5 kHz
 * Timer 1 - PWM mode (8-bit) - RGB leds 					--- expect 7.81 kHz
 * Timer 3 - Interrupt mode, interrupts control servo pins 	--- expect   50  Hz
 * Timer 4 - PWM mode (OC4A & OC4D) 						--- expect 23.4 kHz
 */
 
/* PIN ASSIGNMENTS - Arduino Pin Number - (access register) [range]
 * Motor 1: D13 (OCR4A) [0-1023]
 * Motor 2: D6  (OCR4D) [0-1023]
 * Servo 1: D5 (OCR3A) [0-19999]; microSeconds
 * Servo 2: D4 (OCR3B) [0-19999]; microSeconds; ***software (interrupt) -based
 * RGB Red: D9  (OCR1A) [0-255]
 * RGB Grn: D10 (OCR1B) [0-255]
 * RGB Blu: D11 (OCR1C) [0-255]
 * 38 kHz : D3
 *
 * If possible we should use something like:
 * #define MOTOR1_SPEED OCR4A
 */
 
 void initializeTimers()
 {
	 /* After examining the boards.txt file in the Arduino folder, I have realized that the CPU clock of
	  * 16MHz is divided by 8 (the CLKDIV8 fuse bit is programmed) before being fed to the timers.
	  * This affects the prescaler values, as well as the accuracy of our 38kHz signal.
	  *
	  * Unfortunately it cannot easily be changed. */
	 
	 /* enable global interrupts */
	 sei();
	 
	 
	 
	 /* Initialize Timer 0 for 38kHz IR signal generator */	 
	 OCR0A = 25; /* this cycles the timer at 38.46kHz*/
	 OCR0B = 12; /* this clears Dig. pin 3 (PD0) halfway through the cycle */
	 
	 TCCR0A = 0x00; /* clear all bits */
		TCCR0A |= (1<<COM0B1) + (1<<WGM01) + (1<<WGM00); /* clear PD0 on compare match; fPWM, OCR0A as TOP */	 
		DDRD |= _BV(0); /* Set dig. pin 3 (PD0) to output mode. */
		
	 TCCR0B = 0x00; /* clear all bits */
		TCCR0B  |= (1<<WGM03) + (1<<CS00); /* fastPWM-OCR0A.top; set prescaler to 1 */
	 /* End Timer 0 */ 
	 
	 
	 /* Initialize Timer 1 for 8-bit PWM */
	 OCR1A = 0x0080; /* initialize all OCRs to 50% duty cycle (0x0080) */
	 OCR1B = 0x0080;
	 OCR1C = 0x0080;
	 
	 TCCR1A = 0x00; /* clear all bits */
		TCCR1A |= (1<<COM1A1) + (1<<COM1B1) + (1<<COM1C1) + (1<<WGM10); /* clear on compare match; 8-bitFPWM */
		DDRB |= _BV(5) + _BV(6) + _BV(7); /* set dig. pins 9,10,11 (PD5,PD6,PD7) to output mode */
		
	 TCCR1B = 0x00; /* clear all bits */
		TCCR1B |= (1<<WGM12) + (1<<CS10); /* 8-bitFastPWM; prescaler = 1 (expected freq = 7.8kHz) */
	/* End Timer 1 */ 
	 
	 
	 /* Initialize Timer 3 for a 20ms period, with direct servo control output on OC3A and indirect servo
	  * control via interrupt generated by OCR3B compare match */
	 OCR3A = 0x00; /* IMPORTANT - initialize OCRs to 0 to prevent weirdness */
	 OCR3B = 0x00;
	 ICR3 = 19999; /* this sets the counter period to 20ms */
	 
	 TCCR3A = 0x00; /* clear all bits */
		TCCR3A |= (1<<COM3A1); /* clears OC3A (dig. pin 5) on compare match */
		DDRC |= _BV(6); /* Set dig. pin 5 (PC6) to output mode */
		
	 TCCR3B = 0x00;  /* clear all bits */
		TCCR3B |= (1<<WGM33) + (1<<WGM32) + (1<<CS30); /* CTC mode with period defined by ICR3; prescaler = 1 */
		
	 TCCR3C = 0x00;  /* clear all bits */
		TCCR3C |= (1<<FOC3A); /* enables digital pin 5 to function with zero software overhead (no interrupt) */
		
	 TIMSK3 = 0x00;  /* clear all bits */ 
		TIMSK3 |= (1<<ICIE3) + (1<<OCIE3B); /* enable interrupts for count==ICR3 and count==OCR3B */
		DDRD |= _BV(4); /* Set dig. pin 4 (PD4) to output mode*/
	 /* End Timer 3 */
	 
	 
	 /* Initialize Hi-speed Timer 4 for 10-bit PWM on OC4A (Dig. pin 13) & OC4D (Dig. pin 6) */
	 OCR4C = 1023; /* OCR4C defines the "TOP" value for Timer/Counter 4 */
	 OCR4A = 512;
	 OCR4D = 512;
	 
	 TCCR4A = 0x00; /* clear all bits */
		TCCR4A |= (1<<COM4A1) + (1<<PWM4A); /* clear OC4A (Dig. pin 13) on compare match; enable OC4A PWM */
		DDRC |= _BV(7); /* Set dig. pin 13 (PC7) to output mode*/
		
	 TCCR4B = 0x00; /* clear all bits */
		TCCR4B |= (1<<CS40); /* prescaler = 1 */
		
	 TCCR4C = 0x00; /* clear all bits */
		TCCR4C |= (1<<COM4D1) + (1<<PWM4D); /* clear OC4D (Dig. pin 6) on compare match; enable OC4D PWM */
		DDRD |= _BV(7); /* Set dig. pin 6 (PD7) to output mode*/
		
	 TCCR4D = 0x00; /* clear all bits */
	 TCCR4E = 0x00; /* clear all bits */
	 TIMSK4 = 0x00; /* clear all bits */
	 
	 /* PLL control --- This may or may not affect USB functionality... */
	 /* In all likelihood, connecting or disconnecting USB will screw up Timer4 operation until next reset.
 	  * If USB is connected and enabled, the PLL will need no further configuration. Otherwise it will need
	  * to be set up manually: */
	 if ((USBCON >> USBE) == 0)
	 {
		 /* if USB is disabled (i.e. USBE bit in USBCON is 0), enable the PLL */
		 PLLFRQ = 0x00;  /* clear all bits */
			PLLFRQ |= (1<<PLLUSB) + (1<<PLLTM1) + (1<<PLLTM0) + (1<<PDIV3) + (1<<PDIV1);
			/* halves frequency sent to USB; halves frequency sent to Timer4; Sets raw frequency to 96MHz */
		 
		 /* CODE COPIED FROM "USBCore.cpp" */
		 PLLCSR |= (1<<PINDIV); /* divide 16MHz cpu clock by 2 for input into PLL */
		 PLLCSR |= (1<<PLLE);
		 while (!(PLLCSR & (1<<PLOCK)))		// wait for lock pll
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
	 /* This interrupt is triggered when Timer3 reaches the value in ICR3 (19,999 in this case).
	  * We set the output pin (Dig. pin 4) high at the end (aka beginning) of each timer cycle.
	  *
	  * I'm uncertain of whether this might occur anyway when the counter is cleared. */
	 PORTD |= _BV(4); /* Set Dig. pin 4 (PD4) HIGH */
 }
 
 ISR(TIMER3_COMPB_vect)
  {
	 /* This interrupt is triggered when Timer3 reaches the value in OCR3B.
	  * This time we clear the output pin (Dig. pin 4) at the end of each pulse. */
	 PORTD &= ~(_BV(4)); /* Set Dig. pin 4 (PD4) LOW */
 }