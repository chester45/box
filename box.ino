#include <avr/sleep.h>
#include <Arduino.h>
#include <Servo.h>
#include "Box.h"
#include "log.h"
#include "TimerObjectManager.h"

#define STATUS_LED_PIN  (13)

#define UNUSED_PARAM(x) (void)(x);

#define LED_CLEAR(pin)  digitalWrite(pin, 0)
#define LED_SET(pin)    digitalWrite(pin, 1)
#define LED_TOGGLE(pin) digitalWrite(pin, !digitalRead(pin))

TimerObjectManager *TimerManager = TimerObjectManager::GetManager();
Box::Box box(DEFAULT_USER_SWITCH, DEFAULT_COVER_SERVO_PIN, DEFAULT_STICK_SERVO_PIN);
uint8_t StatusLedTimerIdx = INVALID_TIMER_IDX;

static void write_log(char *buf)
{
    Serial.print(buf);
}

static void interrputCallback(void)
{
}

void StatusLedTimerCallback(void *param)
{
    UNUSED_PARAM(param)

    LED_TOGGLE(STATUS_LED_PIN);
}

static void sleepSetup()
{
    LOG("zzzZZZzzz...\n");
    Serial.flush();                 // flush serial port before sleep
    cli();
    set_sleep_mode(SLEEP_MODE_STANDBY);
    sleep_enable();
    attachInterrupt(digitalPinToInterrupt(box.GetSwitchPin()),
                    interrputCallback, LOW);    // user switch can wakeup
    sei();
    sleep_mode();
    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(box.GetSwitchPin()));
}

void setup()
{
    Serial.begin(9600);
    log_initialize(write_log);
    pinMode(STATUS_LED_PIN, OUTPUT);
    LOG("Setup complete\n");
    StatusLedTimerIdx = TimerManager->CreateTimer(1000, NULL, StatusLedTimerCallback, false);
    if (StatusLedTimerIdx != INVALID_TIMER_IDX)
    {
        TimerManager->StartTimer(StatusLedTimerIdx);
    }
}

void loop()
{
    box.Check();
    TimerManager->UpdateTimers();
    sleepSetup();
    LOG("Active again !\n");
}