#ifndef BOX_H_
#define BOX_H_

#include <stdint.h>
#include <Servo.h>
#include "TimerObject.h"

#define SRV_COVER   1
#define SRV_LEVER   2
#define LED_SWITCH  3
#define LED_AMBIENT 4

#define DEFAULT_USER_SWITCH             2
#define DEFAULT_COVER_SERVO_PIN         9
#define DEFAULT_ARM_SERVO_PIN           10

//On most Arduino boards (those with the ATmega168 or ATmega328P), this function works on pins 3, 5, 6, 9, 10, and 11.

enum MoveSpeed_t {
    IMMEDIATE = 0,
    FAST = 1,
    NORMAL = 2,
    SLOW = 3,
    INVALID
};

typedef struct {
    uint8_t move_1;
    uint8_t speed_1;
    uint8_t move_2;
    uint8_t speed_2;
    uint8_t repeat;
} ServoSequence_t;

typedef struct {
    uint8_t move;
    uint8_t speed;
} Movement_t;

typedef struct {
    uint8_t move_cnt;
    uint8_t repeat;
    Movement_t moves[2];
} Sequence_t;

namespace Box
{
class Box {

public:
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
    uint8_t armServoPin;
    Servo coverServo;
    Servo armServo;
    BoxState_t state;
    void SetState(const BoxState_t newState) {state = newState;}
    void MoveServo(Servo &servo, uint8_t position, MoveSpeed_t speed);
    void RunNormalSequence();
    void RunFastSequence();
    void RunSlowSequence();

public:

    Box(uint8_t switchPin, uint8_t coverServoPin, uint8_t armServoPin);
    void timerDelayCallback();
    void timerSwitchPwmCallback();
    void Setup();
    void Check();
    bool IsIdle() const;
    uint8_t GetSwitchPin() const;
    void DebugMoveServo(uint8_t servoNum, uint8_t position, MoveSpeed_t speed);
    uint8_t DebugGetServoPosition(uint8_t servoNum);
};
}

#endif