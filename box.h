#ifndef BOX_H_
#define BOX_H_

#include <stdint.h>
#include "TimerObject.h"

#define SRV_COVER   1
#define SRV_LEVER   2
#define LED_SWITCH  3
#define LED_AMBIENT 4

#define DEFAULT_USER_SWITCH 2

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

    uint8_t UserSwitchPin;

    BoxState state;
    TimerObject *timer_delay;
    TimerObject *timer_swtich_pwm;

    void setState(const BoxState new_state) {state = new_state;}

public:
    Box(uint8_t SwitchPin);
    void timerDelayCallback();
    void timerSwitchPwmCallback();
    void setup();
    void check();
    bool isIdle() const;
    uint8_t getSwitchPin() const;
    static void staticDelayCallback(void *box) {static_cast<Box*>(box)->timerDelayCallback();};
    static void staticSwitchPwmCallback(void *box) {static_cast<Box*>(box)->timerSwitchPwmCallback();};
};

}


#endif