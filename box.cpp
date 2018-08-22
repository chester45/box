#include <Arduino.h>
#include "Box.h"
#include "log.h"

namespace Box
{

#define BOX_DELAY (2000) // 2 sec
#define PWM_DELAY (1000) // 1 sec

const uint8_t Box::SpeedLUT[INVALID] = {0, 5, 25, 50};

Box::Box(uint8_t SwitchPin, uint8_t CoverServoPin, uint8_t StickServoPin)
{
    state = BoxState::IDLE;
    UserSwitchPin = SwitchPin;
    this->CoverServoPin = CoverServoPin;
    this->StickServoPin = StickServoPin;
    Setup();
}

void Box::Setup()
{
    pinMode(UserSwitchPin, INPUT_PULLUP);
    CoverServo.attach(CoverServoPin);
    StickServo.attach(StickServoPin);
    CoverServo.WriteDelay(0);
    StickServo.WriteDelay(0);
}

void Box::Check()
{
    uint8_t pin_state = digitalRead(UserSwitchPin);

    switch (state)
    {
        case BoxState::IDLE:
            if (!pin_state)
            {
                //SetState(BoxState::WAIT);
            }
        break;
        case BoxState::WAIT:
            if (!pin_state)
            {}
        break;
        default:
            break;
    }
}

bool Box::IsIdle() const
{
    return (state == BoxState::IDLE);
}

uint8_t Box::GetSwitchPin() const
{
    return UserSwitchPin;
}

void Box::MoveServo(MyServo &Servo, uint8_t NewPosition, MoveSpeed Speed)
{
    uint8_t DelayTime = SpeedLUT[Speed];
    uint8_t CurrentPosition = Servo.GetCurrentPosition();

    while(CurrentPosition != NewPosition)
    {
        (CurrentPosition > NewPosition) ? CurrentPosition-- : CurrentPosition++;
        Servo.write(CurrentPosition);
        delay(DelayTime);
    }
}

}