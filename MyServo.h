#include <Arduino.h>
#include <Servo.h>
#include "TimerObjectManager.h"

class MyServo : public Servo
{
    public:

    enum class MoveSpeed{
        FAST,
        NORMAL,
        SLOW
    };

    MyServo();
    void WriteDelay(int value);
    void Move(int value, MoveSpeed Speed);

    private:
        uint8_t DelayTimer;
        uint8_t GetMoveDelay(MoveSpeed);
};