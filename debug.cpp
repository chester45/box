#include <Arduino.h>
#include <avr/wdt.h>
#include "debug.h"
#include "box.h"
#include "log.h"

DEFINE_DEBUG_TAG("[DBG]");

/**Local definitions**/
static void ResetCommandInfo(void);
static void LogInvalidParams(void);
static void ParseCommand(void);
static void ExecuteCommand(void);
static void ExecuteHelpCmd(void);
static void ExecuteDebugCmd(void);
static void ExecuteMoveServoCmd(void);
static void ExecuteGetServoPosCmd(void);
static void ExecuteResetCmd(void);

static Box::Box *box_p = NULL;
static bool DebugModeEnabled = false;
String cmdString;
Command_t debugCmd;

void (*ResetFunc)(void) = 0;

DebugCommand_t debugCmdTable [] =
{
    {'h', ExecuteHelpCmd,        "Print help: h"},
    {'d', ExecuteDebugCmd,       "Enable debug mode: d [0/1]"},
    {'s', ExecuteMoveServoCmd,   "Set servo position: s [srv_num] [pos] [speed]"},
    {'g', ExecuteGetServoPosCmd, "Get servo postiion: s [srv_num]"},
    {'r', ExecuteResetCmd,       "Reset the device: r [1]"},
    // must always be last
    {' ', NULL, NULL}
};

/************************************************************************
* Public functions
************************************************************************/

void InitDebugModule(Box::Box *box)
{
    if (!box)
    {
        LOG("Failed to init debug module\n");
        return;
    }
    ResetCommandInfo();
    box_p = box;
    DebugModeEnabled = false;
}

bool IsDebugModeEnabled()
{
    return DebugModeEnabled;
}

void FetchDebugCommand(void)
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

/************************************************************************
* Local functions
************************************************************************/
static void ResetCommandInfo()
{
    cmdString.remove(0);
    debugCmd.cmd = ' ';
    debugCmd.validParams = 0;
    debugCmd.params[0] = 0;
    debugCmd.params[1] = 0;
    debugCmd.params[2] = 0;
}

static void LogInvalidParams()
{
    LOG("Invalid cmd params: %d %d %d\n", debugCmd.params[0], debugCmd.params[1], debugCmd.params[0]);
}

static void ParseCommand()
{
    if (cmdString.length() < 3)
    {
        LOG("Wrong length: %d\n", cmdString.length());
        return;
    }

    debugCmd.cmd = cmdString[0];
    int paramStartIdx = 2;

    for (uint8_t i = 0; i < 3; i ++)
    {
        int paramEndIdx = cmdString.indexOf(" ", paramStartIdx);
        if (paramEndIdx == -1)
        {   // This is last parameter
            debugCmd.validParams++;
            debugCmd.params[i] = cmdString.substring(paramStartIdx).toInt();
            break;
        }
        else
        {
            debugCmd.validParams++;
            debugCmd.params[i] = cmdString.substring(paramStartIdx, paramEndIdx).toInt();
            paramStartIdx = paramEndIdx + 1;
        }
    }
    ExecuteCommand();
}

/*
static void PrintCommand()
{
    LOG("Command: %c, %d, %d, %d, %d\n", debugCmd.cmd,
                                         debugCmd.validParams,
                                         debugCmd.params[0],
                                         debugCmd.params[1],
                                         debugCmd.params[2]);
}
*/

static void ExecuteCommand()
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
        LOG("Invalid command\n");
}

static void ExecuteHelpCmd()
{
    uint8_t idx = 0;
    while (debugCmdTable[idx].cmd != ' ')
    {
        LOG_BASIC("\t%c", debugCmdTable[idx].cmd);
        if (debugCmdTable[idx].h_str)
        {
            LOG_BASIC(" %s", debugCmdTable[idx].h_str);
        }
        else
        {
            LOG_BASIC(" no help available");
        }
        LOG_BASIC("\n");
        idx++;
    }
}

static void ExecuteDebugCmd(void)
{
    if (debugCmd.params[0] == 1)
    {
        LOG("Debug mode enabled\n");
        DebugModeEnabled = 1;
    }
    else if (debugCmd.params[0] == 0)
    {
        LOG("Debug mode disabled\n");
        DebugModeEnabled = 0;
    }
    else
    {
        LogInvalidParams();
    }
}

static void ExecuteMoveServoCmd()
{
    if (debugCmd.params[0] > 1 || debugCmd.params[1] > 180 || debugCmd.params[2] > SLOW)
    {
        LogInvalidParams();
    }
    else
    {
        box_p->DebugMoveServo(debugCmd.params[0], debugCmd.params[1], (MoveSpeed_t)debugCmd.params[2]);
    }
}

static void ExecuteGetServoPosCmd()
{
    if (debugCmd.params[0] > 1)
    {
        LogInvalidParams();
    }
    else
    {
        uint8_t pos = box_p->DebugGetServoPosition(debugCmd.params[0]);
        LOG("Srv: %d, Pos: %d\n", debugCmd.params[0], pos);
    }
}

static void ExecuteResetCmd()
{
    if (debugCmd.params[0] != 1)
    {
        LogInvalidParams();
        return;
    }

    LOG("Resetting platform ... \n");

    // watchdog does not work with nano - some bootloader issues
    //wdt_enable(WDTO_1S);

    delay(3000);
    ResetFunc();

    LOG("Fail to reset ! \n");
}
