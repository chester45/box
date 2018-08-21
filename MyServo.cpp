#include "Arduino.h"
#include "MyServo.h"
#include "TimerObjectManager.h"
#include "log.h"

const uint8_t MyServo::DelayLUT[MoveSpeed::INVALID] = {0, 25, 50};

MyServo::MyServo()
{
    Servo();
}

void MyServo::write(int value)
{
    CurrentPosition = value;
    Servo::write(CurrentPosition);
}

void MyServo::WriteDelay(int value)
{
    write(value);
    // Wait 2ms to give time to reach position
    // TODO: Delay in not nice solution here but should do the job now
    delay(2);
}

void MyServo::Move(int Position, MoveSpeed Speed)
{
    if (Speed >= MoveSpeed::INVALID)
    {
        LOG("Invalid speed value!");
        return;
    }

    NewPostion = (Position > MAX_SERVO_POSITION) ? MAX_SERVO_POSITION : Position;
    uint8_t DelayTime = DelayLUT[Speed];
    if (!DelayTime)
    {   // Deley is 0, so set servo postion immediately
        write(NewPostion);
    }
    else
    {
        while(NewPostion != CurrentPosition)
        {
            Step();
            delay(DelayTime);
        }
    }
}

void MyServo::Step()
{
    if (NewPostion > CurrentPosition)
    {
        write(++CurrentPosition);
    }
    else if (NewPostion < CurrentPosition)
    {
        write(--CurrentPosition);
    }
    LOG("Current Servo position %d", CurrentPosition); // TODO: remove this line
}