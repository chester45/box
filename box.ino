#include <avr/sleep.h>
#include <Arduino.h>
#include <Servo.h>
#include <avr/wdt.h>
#include "Box.h"
#include "log.h"
#include "TimerObjectManager.h"
#include "debug.h"

DEFINE_DEBUG_TAG("[INO]");

#define STATUS_LED_PIN          (13)

#define UNUSED_PARAM(x)         (void)(x);

#define LED_CLEAR(pin)          digitalWrite(pin, 0)
#define LED_SET(pin)            digitalWrite(pin, 1)
#define LED_TOGGLE(pin)         digitalWrite(pin, !digitalRead(pin))

TimerObjectManager *TimerManager = TimerObjectManager::GetManager();
Box::Box *box = NULL;
uint8_t StatusLedTimerIdx = INVALID_TIMER_IDX;

static void IndicateStart()
{
        uint8_t cnt = 6;
        while (cnt--) {
                LED_TOGGLE(STATUS_LED_PIN);
                delay(250);
        }
}

static void InterrputCallback(void)
{
}

void StatusLedTimerCallback(void *param)
{
        UNUSED_PARAM(param)
        LED_TOGGLE(STATUS_LED_PIN);
}

static void SleepSetup()
{
        LOG_PM("zzzZZZzzz...\n");
        Serial.flush();                 // flush serial port before sleep
        cli();
        set_sleep_mode(SLEEP_MODE_STANDBY);
        sleep_enable();
        attachInterrupt(digitalPinToInterrupt(box->GetSwitchPin()), InterrputCallback, LOW);    // user switch can wakeup
        sei();
        sleep_mode();
        sleep_disable();
        detachInterrupt(digitalPinToInterrupt(box->GetSwitchPin()));
}

void setup()
{
        wdt_disable();
        Serial.begin(9600);
        delay(20);
        LOG_PM("Setup start ...\n");
        box = new Box::Box(DEFAULT_USER_SWITCH, DEFAULT_COVER_SERVO_PIN, DEFAULT_ARM_SERVO_PIN);
        InitDebugModule(box);
        pinMode(STATUS_LED_PIN, OUTPUT);

        StatusLedTimerIdx = TimerManager->CreateTimer(1000, NULL, StatusLedTimerCallback, false);
        if (StatusLedTimerIdx != INVALID_TIMER_IDX) {
                IndicateStart();
                TimerManager->StartTimer(StatusLedTimerIdx);
        }
        LOG_PM("Setup complete\n");
}

void loop()
{
        if (!IsDebugModeEnabled()) {
                box->Check();
                TimerManager->UpdateTimers();
                //sleepSetup();
                //LOG("Active again !\n");
        }
}

void serialEvent()
{
        FetchDebugCommand();
}
