#ifndef BOX_H_
#define BOX_H_

#include <stdint.h>
#include "MyServo.h"
#include "TimerObject.h"

#define SRV_COVER   1
#define SRV_LEVER   2
#define LED_SWITCH  3
#define LED_AMBIENT 4

#define DEFAULT_USER_SWITCH         2
#define DEFAULT_COVER_SERVO_PIN     9
#define DEFAULT_STICK_SERVO_PIN     10

//On most Arduino boards (those with the ATmega168 or ATmega328P), this function works on pins 3, 5, 6, 9, 10, and 11.

namespace Box
{
class Box {

public:
    enum MoveSpeed_t{
        IMMEDIATE = 0,
        FAST = 1,
        NORMAL = 2,
        SLOW = 3,
        INVALID
    };

    enum BoxState_t {
        STATE_IDLE,
        STATE_WAIT,
        STATE_SWITCH_CHANGE,
        STATE_IN_ACTION
    };

private:

    static const uint8_t SpeedLUT[INVALID];
    uint8_t userSwitchPin;
    uint8_t coverServoPin;
    uint8_t stickServoPin;
    MyServo coverServo;
    MyServo stickServo;
    BoxState_t state;
    void SetState(const BoxState_t newState) {state = newState;}
    void MoveServo(MyServo &servo, uint8_t position, MoveSpeed_t speed);

public:

    Box(uint8_t switchPin, uint8_t coverServoPin, uint8_t stickServoPin);
    void timerDelayCallback();
    void timerSwitchPwmCallback();
    void Setup();
    void Check();
    bool IsIdle() const;
    uint8_t GetSwitchPin() const;
};
}

#endif