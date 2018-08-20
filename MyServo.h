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
    int CurrentPosition;
    int NewPostion;
    void write(int value);
    void WriteDelay(int value);
    void Move(int Position, MoveSpeed Speed);
    friend void ServoTimerCallback(void *);

    private:
        uint8_t TimerIndex;
        uint8_t GetMoveDelay(MoveSpeed);
        void DelayExpired();
};