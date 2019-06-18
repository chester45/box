#include <Arduino.h>
#include <avr/pgmspace.h>
#include "Box.h"
#include "TimerObjectManager.h"
#include "log.h"

#define COVER_OPEN_POSITION             75
#define COVER_PEEK_POSITION             30
#define COVER_CLOSE_POSITION            0

#define ARM_HIDE_POSITION               0
#define ARM_FULL_EXTEND_POSITION        180

#define MOVE_COVER(pos, speed)          MoveServo(coverServo, pos, speed)
#define MOVE_ARM(pos, speed)            MoveServo(armServo, pos, speed)

#define COVER_OPEN_FAST()               MOVE_COVER(COVER_OPEN_POSITION, FAST)
#define COVER_OPEN_NORMAL()             MOVE_COVER(COVER_OPEN_POSITION, NORMAL)
#define COVER_OPEN_SLOW()               MOVE_COVER(COVER_OPEN_POSITION, SLOW)
#define COVER_CLOSE_FAST()              MOVE_COVER(COVER_CLOSE_POSITION, FAST)
#define COVER_CLOSE_NORMAL()            MOVE_COVER(COVER_CLOSE_POSITION, NORMAL)
#define COVER_CLOSE_SLOW()              MOVE_COVER(COVER_CLOSE_POSITION, SLOW)

#define ARM_FULL_EXTEND_FAST()          MOVE_ARM(ARM_FULL_EXTEND_POSITION, FAST)
#define ARM_FULL_EXTEND_NORMAL()        MOVE_ARM(ARM_FULL_EXTEND_POSITION, NORMAL)
#define ARM_FULL_EXTEND_SLOW()          MOVE_ARM(ARM_FULL_EXTEND_POSITION, SLOW)
#define ARM_HIDE_FAST()                 MOVE_ARM(ARM_HIDE_POSITION, FAST)
#define ARM_HIDE_NORMAL()               MOVE_ARM(ARM_HIDE_POSITION, NORMAL)
#define ARM_HIDE_SLOW()                 MOVE_ARM(ARM_HIDE_POSITION, SLOW)


namespace Box
{

DEFINE_DEBUG_TAG("[BOX]");

#define BOX_DELAY               (2000) // 2 sec
#define PWM_DELAY               (1000) // 1 sec
#define COOL_DOWN_DELAY         (1000)    // 1sec
#define LED_FADE_DEALY          (100)

const uint8_t Box::SpeedLUT[INVALID] PROGMEM = {0, 5, 25, 50};

void CoolDownTimerCallback(void*);
void LedFadeTimerCallback(void*);

Box::Box(uint8_t switchPin, uint8_t coverServoPin, uint8_t armServoPin, uint8_t ledPin)
{
        userSwitchPin = switchPin;
        this->coverServoPin = coverServoPin;
        this->armServoPin = armServoPin;
        this->ledPin = ledPin;
        angryLevel = 0;
        coolDownTimer = TimerObjectManager::GetManager()->CreateTimer(COOL_DOWN_DELAY, this,
                                                                CoolDownTimerCallback, false);
        ledFadeTimer = TimerObjectManager::GetManager()->CreateTimer(LED_FADE_DEALY, this,
                                                                LedFadeTimerCallback, false);
        Setup();
}

void Box::Setup()
{
        pinMode(userSwitchPin, INPUT_PULLUP);
        pinMode(ledPin, OUTPUT);
        coverServo.attach(coverServoPin);
        armServo.attach(armServoPin);
        coverServo.write(0);
        armServo.write(0);
}

void Box::Check()
{
        uint8_t pinState = digitalRead(userSwitchPin);

        if (!pinState)
                TriggerAction();
}

void Box::TriggerAction()
{
        IncreaseAngryLevel();
}

bool Box::IsIdle() const
{
        return ((angryLevel) ? true : false);
}

uint8_t Box::GetSwitchPin() const
{
        return userSwitchPin;
}

void Box::MoveServo(Servo &servo, uint8_t newPosition, MoveSpeed_t speed)
{
        uint8_t delayTime = pgm_read_byte(SpeedLUT + speed);
        uint8_t currentPosition = servo.read();

        while(currentPosition != newPosition) {
                (currentPosition > newPosition) ? currentPosition-- : currentPosition++;
                servo.write(currentPosition);
                delay(delayTime);
        }
}

void Box::CoolDownEvent()
{

}

void Box::LedFadeEvent()
{

}

void Box::IncreaseAngryLevel()
{
        if (angryLevel < 255)
                angryLevel++ ;
}

void Box::DecreaseAngryLevel()
{
        if (angryLevel)
                angryLevel-- ;
}

void Box::SetLedBrightness(uint8_t value)
{
        analogWrite(ledPin, value);
}

void Box::DebugMoveServo(uint8_t servoNum, uint8_t position, MoveSpeed_t speed)
{
        Servo &srv = (!servoNum) ? coverServo : armServo;
        MoveServo(srv, position, speed);
}

uint8_t Box::DebugGetServoPosition(uint8_t servoNum)
{
        Servo &srv = (!servoNum) ? coverServo : armServo;
        return (srv.read());
}

void Box::DebugSetLedBrightness(uint8_t value)
{
        SetLedBrightness(value);
}

void Box::RunNormalSequence()
{
        COVER_OPEN_NORMAL();
        ARM_FULL_EXTEND_NORMAL();
        ARM_HIDE_NORMAL();
        COVER_CLOSE_NORMAL();
}

void Box::RunFastSequence()
{
        COVER_OPEN_FAST();
        ARM_FULL_EXTEND_FAST();
        ARM_HIDE_FAST();
        COVER_CLOSE_FAST();
}

void Box::RunSlowSequence()
{
        COVER_OPEN_SLOW();
        ARM_FULL_EXTEND_SLOW();
        ARM_HIDE_SLOW();
        COVER_CLOSE_SLOW();
}

void CoolDownTimerCallback(void *param)
{
        Box *obj = static_cast<Box*>(param);
        obj->CoolDownEvent();
}

void LedFadeTimerCallback(void *param)
{
        Box *obj = static_cast<Box*>(param);
        obj->LedFadeEvent();
}

}