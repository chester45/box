#include <Arduino.h>
#include "Box.h"
#include "log.h"

namespace Box
{

#define BOX_DELAY (2000) // 2 sec
#define PWM_DELAY (1000) // 1 sec

const uint8_t Box::SpeedLUT[INVALID] = {0, 5, 25, 50};

Box::Box(uint8_t switchPin, uint8_t coverServoPin, uint8_t stickServoPin)
{
    state = STATE_IDLE;
    userSwitchPin = switchPin;
    this->coverServoPin = coverServoPin;
    this->stickServoPin = stickServoPin;
    Setup();
}

void Box::Setup()
{
    pinMode(userSwitchPin, INPUT_PULLUP);
    coverServo.attach(coverServoPin);
    stickServo.attach(stickServoPin);
    coverServo.write(0);
    stickServo.write(0);
}

void Box::Check()
{
    uint8_t pinState = digitalRead(userSwitchPin);

    switch (state)
    {
        case STATE_IDLE:
            if (!pinState)
            {
                //SetState(BoxState::WAIT);
            }
        break;
        case STATE_WAIT:
            if (!pinState)
            {

            }
        break;
        default:
            break;
    }
}

bool Box::IsIdle() const
{
    return (state == STATE_IDLE);
}

uint8_t Box::GetSwitchPin() const
{
    return userSwitchPin;
}

void Box::MoveServo(Servo &servo, uint8_t newPosition, MoveSpeed_t speed)
{
    uint8_t delayTime = SpeedLUT[speed];
    uint8_t currentPosition = servo.read();

    while(currentPosition != newPosition)
    {
        (currentPosition > newPosition) ? currentPosition-- : currentPosition++;
        servo.write(currentPosition);
        delay(delayTime);
    }
}

}