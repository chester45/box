#include "Arduino.h"
#include "MyServo.h"
#include "TimerObjectManager.h"
#include "log.h"

const uint8_t MyServo::DelayTime[MoveSpeed::INVALID] = {0, 25, 50};

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
    if (Speed >= MoveSpeed::INVALID)
    {
        LOG("Invalid speed value!");
        return;
    }

    NewPostion = (Position > MAX_SERVO_POSITION) ? MAX_SERVO_POSITION : Position;
    uint8_t delay = DelayTime[Speed];
    if (!delay)
    {   // Deley is 0, so set servo postion immediately
        write(NewPostion);
    }
    else
    {
        // Starting timer to get slower but smooth servo movement
        TimerObjectManager *TimerManager = TimerObjectManager::GetManager();
        TimerManager->SetInterval(TimerIndex, delay);
        TimerManager->StartTimer(TimerIndex);
    }
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
    { //Servo has reached its postion so stop the timer
        TimerObjectManager::GetManager()->StopTimer(TimerIndex);
    }
}