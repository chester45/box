#include <Arduino.h>
#include <Servo.h>
#include <StandardCplusplus.h>
#include <map>

#include "TimerObjectManager.h"

#define MAX_SERVO_POSITION  180

class MyServo : public Servo
{
    public:

    enum MoveSpeed{
        FAST = 0,
        NORMAL = 1,
        SLOW = 2,
        INVALID
    };

    MyServo();
    int CurrentPosition;
    int NewPostion;
    void write(int value);
    void WriteDelay(int value);
    void Move(int Position, MoveSpeed Speed);

    private:
        void Step();
        static const uint8_t DelayLUT[INVALID];
};