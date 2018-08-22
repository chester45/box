#include "Arduino.h"
#include "MyServo.h"
#include "TimerObjectManager.h"
#include "log.h"

MyServo::MyServo()
{
    Servo();
}

void MyServo::write(int value)
{
    CurrentPosition = (value <= MAX_SERVO_POSITION) ? value : MAX_SERVO_POSITION;
    Servo::write(CurrentPosition);
}

void MyServo::WriteDelay(int value)
{
    Servo::write(value);
    // Wait 2ms to give time to reach position
    // TODO: Delay in not nice solution here but should do the job now
    delay(2);
}