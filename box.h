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

private:
    enum class BoxState {
        IDLE,
        WAIT,
        SWITCH_CHANGE,
        IN_ACTION
    };

    enum MoveSpeed{
        IMMEDIATE = 0,
        FAST = 1,
        NORMAL = 2,
        SLOW = 3,
        INVALID
    };

    static const uint8_t SpeedLUT[INVALID];
    uint8_t UserSwitchPin;
    uint8_t CoverServoPin;
    uint8_t StickServoPin;
    MyServo CoverServo;
    MyServo StickServo;
    BoxState state;
    void SetState(const BoxState new_state) {state = new_state;}
    void MoveServo(MyServo &Servo, uint8_t Position, MoveSpeed Speed);

public:
    Box(uint8_t SwitchPin, uint8_t CoverServoPin, uint8_t StickServoPin);
    void timerDelayCallback();
    void timerSwitchPwmCallback();
    void Setup();
    void Check();
    bool IsIdle() const;
    uint8_t GetSwitchPin() const;
};
}

#endif