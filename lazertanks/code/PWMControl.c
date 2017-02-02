//#define LEFTM_INVERTED 1
//#define RIGHTM_INVERTED 1
// uncomment the relevant #define to invert the motor phase

#define LEFTM_PHASE_PIN 7
#define RIGHTM_PHASE_PIN 8

#define TURRET_ZERO_VALUE 3000
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

/*void moveBarrel(int bPosition)
{
	bPosition = constrain(bPosition, BARREL_MINVAL, BARREL_MAXVAL);
	
#ifdef BARREL_INVERT_DIR	
	bPosition = BARREL_MAXVAL + BARREL_MINVAL - bPosition;
#endif	
	
	OCR3B = bPosition;
}*/

void moveBarrel(int bInputVal)
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

}

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



















