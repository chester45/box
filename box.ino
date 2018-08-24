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
Box::Box box(DEFAULT_USER_SWITCH, DEFAULT_COVER_SERVO_PIN, DEFAULT_ARM_SERVO_PIN);
uint8_t StatusLedTimerIdx = INVALID_TIMER_IDX;
bool DebugModeEnabled = false;


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

typedef void (*CmdFunc_t)(void);

typedef struct
{
    char cmd;
    CmdFunc_t execFunc;
} DebugCommand_t;

typedef struct
{
    char cmd;
    uint8_t validParams;
    int params[3];
}  Command_t;

String cmdString;
Command_t debugCmd;

DebugCommand_t debugCmdTable [] =
{
    // must always be last
    {' ', NULL}
};

void ResetCommandInfo()
{
    cmdString.remove(0);
    debugCmd.cmd = ' ';
    debugCmd.validParams ++;
    debugCmd.params[0] = 0;
    debugCmd.params[1] = 0;
    debugCmd.params[2] = 0;

}

void LogInvalidParams()
{
    LOG("[DBG] Invalid cmd params: %d %d %d", debugCmd.params[0], debugCmd.params[1], debugCmd.params[0]);
}

void ExecuteCommand()
{
    uint8_t idx = 0;
    bool cmdFound = false;
    while (debugCmdTable[idx].cmd != ' ')
    {
        if (debugCmd.cmd == debugCmdTable[idx].cmd)
        {
            if (debugCmdTable[idx].execFunc != NULL)
            {
                debugCmdTable[idx].execFunc();
                cmdFound = true;
            }
            break;
        }
        idx++;
    }

    if (!cmdFound)
        LOG("[DBG] Invalid command\n");
}

void ParseCommand()
{
    if (cmdString.length() < 3)
    {
        LOG("[DBG] Wrong length: %d\n", cmdString.length());
        return;
    }

    debugCmd.cmd = cmdString[0];
    int paramStartIdx = 3;

    for (uint8_t i = 0; i < 3; i ++)
    {
        int paramEndIdx = cmdString.indexOf(" ", paramStartIdx);
        debugCmd.validParams++;
        if (paramEndIdx == -1)
        {   // This is last parameter
            debugCmd.params[i] = cmdString.substring(paramStartIdx).toInt();
            break;
        }
        else
        {
            debugCmd.params[i] = cmdString.substring(paramStartIdx, paramEndIdx).toInt();
            paramStartIdx = paramEndIdx + 1;
        }
    }
    ExecuteCommand();
}

void serialEvent()
{
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == '\n')
    {
        ParseCommand();
        ResetCommandInfo();
    }
    else
    {
        cmdString += inChar;
    }
  }
}