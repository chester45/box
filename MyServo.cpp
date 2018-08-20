#include "Arduino.h"
#include "MyServo.h"
#include "TimerObjectManager.h"
#include "log.h"

void ServoTimerCallback(void *Param)
{
    MyServo *Servo_p = static_cast<MyServo*>(Param);
    Servo_p->DelayExpired();
}

MyServo::MyServo()
{
    Servo();
    TimerIndex = TimerObjectManager::GetManager()->CreateTimer(0, this, ServoTimerCallback, false);
    if (TimerIndex == INVALID_TIMER_IDX)
    {
        LOG("Failed to create Servo timer !");
    }
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
    NewPostion = Position;
    uint8_t delay = GetMoveDelay(Speed);
}

uint8_t MyServo::GetMoveDelay(MoveSpeed Speed)
{
    return 20;  // TODO: Adjust return values
}

void MyServo::DelayExpired()
{
    if (NewPostion > CurrentPosition)
    {
        write(++CurrentPosition);
    }
    else if (NewPostion > CurrentPosition)
    {
        write(--CurrentPosition);
    }
    else
    {
        TimerObjectManager::GetManager()->StopTimer(TimerIndex);
    }
}