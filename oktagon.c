// constants
const int A4988_Direction = 3;
const int A4988_Step      = 4;
const int A4988_Enable    = 5;
const int Arduino_Button  = 6;
const int stepsPerRevolution = 200;         // Motor steps per revolution (adjust based on your motor's specs)
const unsigned long  MinStepPulseW_us = 5;  // previously 1000µs, reduced due to suggestion
// vars
unsigned long  lastPressTime_ms   = 0;
unsigned long  WaitTilFinish_ms   = 400;
unsigned long  stepDelay          = 0;
unsigned short randomRepCount     = 0;      // restart rotations this many times
unsigned short rpm                = 0;      // Motor rotations per minute

void setup()
{
    pinMode(A4988_Direction, OUTPUT);       // Set pin direction as output
    pinMode(A4988_Step,      OUTPUT);       // Set pin step as output
    pinMode(A4988_Enable,    OUTPUT);       // Set pin enable as output
    pinMode(Arduino_Button,  INPUT_PULLUP); // Set pin button as input with internal pull-up

    digitalWrite(A4988_Enable,   HIGH);     // Set pin enable initially to HIGH (motor disabeled)
    digitalWrite(A4988_Direction,HIGH);     // Set pin HIGH / LOW depending on your desired direction
}

void loop()
{
    // state is notActive
    if (0 == randomRepCount)
    {
        // button press && hysteresis
        if ((LOW == digitalRead(Arduino_Button))
        && ((millis() - lastPressTime_ms) >= WaitTilFinish_ms))
        {
            startMotor();
            randomRepCount = random(0,8);     // 0,1,2,3,4,5,6,7
        }
    }
    // Continue motor operation
    else
    {
        stopMotor();
        randomRepCount--;

        if (0 < randomRepCount)
        {
            startMotor();
        }
    }
}

void startMotor()
{
    digitalWrite(A4988_Direction, random(0,2)); // 0=left  1=right
    digitalWrite(A4988_Enable, LOW);             // Set pin enable to LOW (motor enabeled)

    rpm              = random(10, 80);      // 10...79
    stepDelay        = (100L * 1000000L) / (rpm * stepsPerRevolution);

    // ramp up
    for (unsigned long rampDelay = 2 * stepDelay; rampDelay >= stepDelay; rampDelay -=10)
    {
        makeStep(rampDelay);
    }

    WaitTilFinish_ms = random(400, 1600);   // 400...1599
    lastPressTime_ms = millis();            // remember when button press occured
}

void stopMotor()    
{
    // ramp down
    for (unsigned long rampDelay = stepDelay; rampDelay < (2 * stepDelay); rampDelay +=10)
    {
        makeStep(rampDelay);
    }

    digitalWrite(A4988_Enable, HIGH);      // Set pin enable initially to HIGH (motor disabeled)
    delay(4000);
}

void makeStep(unsigned long _stepDelay)
{
    // Make one step
    digitalWrite(A4988_Step, HIGH);
    delayMicroseconds(MinStepPulseW_us);
    digitalWrite(A4988_Step, LOW);

    delayMicroseconds(_stepDelay - MinStepPulseW_us); // Subtract pulse width to keep timing accurate
}
