#include "Box.h"
#include "log.h"
#include <avr/sleep.h>
#include <Arduino.h>

Box::Box box(DEFAULT_USER_SWITCH);

static void write_log(char *buf)
{
    Serial.print(buf);
}

static void interrputCallback(void)
{
    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(box.GetSwitchPin()));
}

static void sleepSetup()
{
    LOG("zzzZZZzzz...\n");
    Serial.flush();                 // flush serial port before sleep
    cli();
    sleep_enable();
    attachInterrupt(digitalPinToInterrupt(box.GetSwitchPin()),
                    interrputCallback, LOW);    // user switch can wakeup
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sei();
    sleep_cpu();
    LOG("Active again !\n");
}

void setup()
{
    Serial.begin(9600);
    log_initialize(write_log);
    LOG("Setup complete\n");
}

void loop()
{
    box.Check();
    if (box.IsIdle())
    {
        LOG("Going to sleep\n");
        sleepSetup();
    }
}