#include <Arduino.h>
#include <Servo.h>
#include <StandardCplusplus.h>
#include <map>

#include "TimerObjectManager.h"

#define MAX_SERVO_POSITION  180

class MyServo : public Servo
{
    public:

    MyServo();
    void write(int value);
    void WriteDelay(int value);
    int GetCurrentPosition() {return currentPosition;};

    private:
        int currentPosition;

};