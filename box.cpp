#include <Arduino.h>
#include "Box.h"
#include "log.h"

namespace Box
{

#define BOX_DELAY (2000) // 2 sec
#define PWM_DELAY (1000) // 1 sec

Box::Box(uint8_t SwitchPin)
{
    state = BoxState::IDLE;
    timer_delay = new TimerObject(BOX_DELAY, this, Box::staticDelayCallback, false);
    UserSwitchPin = SwitchPin;
    setup();
}

void Box::timerDelayCallback()
{
    LOG("delay callback\n");
    setState(BoxState::IDLE);
    timer_delay->Stop();
}

void Box::timerSwitchPwmCallback()
{
    LOG("PWM callback");
}

void Box::setup()
{
    pinMode(UserSwitchPin, INPUT_PULLUP);
}

void Box::check()
{
    uint8_t pin_state = digitalRead(UserSwitchPin);
    timer_delay->Update();
    timer_swtich_pwm->Update();
    switch (state)
    {
        case BoxState::IDLE:
            if (!pin_state)
            {
                timer_delay->Start();
                setState(BoxState::WAIT);
            }
        break;
        case BoxState::WAIT:
            if (!pin_state)
            {
                timer_delay->Start();
            }
        break;
        default:
            break;
    }
}

bool Box::isIdle() const
{
    return (state == BoxState::IDLE);
}

uint8_t Box::getSwitchPin() const
{
    return UserSwitchPin;
}

}