#include "Arduino.h"
#include "MyServo.h"
#include "TimerObjectManager.h"

MyServo::MyServo()
{
    Servo();
}

void MyServo::WriteDelay(int value)
{
    write(value);
    // Wait 2ms to give time to reach position
    // TODO: Delay in not nice solution here but should do the job now
    delay(2);
}

void MyServo::Move(int value, MoveSpeed Speed)
{


}

uint8_t MyServo::GetMoveDelay(MoveSpeed Speed)
{

}