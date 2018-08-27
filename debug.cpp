#include <Arduino.h>
#include "debug.h"
#include "box.h"
#include "log.h"

/**Local definitions**/
static void ResetCommandInfo(void);
static void LogInvalidParams(void);
static void ParseCommand(void);
static void ExecuteCommand(void);
static void ExecuteDebugCmd(void);
static void ExecuteMoveServoCmd(void);
static void ExecuteGetServoPosCmd(void);

static Box::Box *box_p = NULL;
static bool DebugModeEnabled = false;
String cmdString;
Command_t debugCmd;

DebugCommand_t debugCmdTable [] =
{
    {'d', ExecuteDebugCmd},
    {'s', ExecuteMoveServoCmd},
    {'g', ExecuteGetServoPosCmd},
    // must always be last
    {' ', NULL}
};

/************************************************************************
* Public functions
************************************************************************/

void InitDebugModule(Box::Box *box)
{
    if (!box)
    {
        LOG("[DBG] Failed to init debug module");
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
    debugCmd.validParams ++;
    debugCmd.params[0] = 0;
    debugCmd.params[1] = 0;
    debugCmd.params[2] = 0;
}

static void LogInvalidParams()
{
    LOG("[DBG] Invalid cmd params: %d %d %d", debugCmd.params[0], debugCmd.params[1], debugCmd.params[0]);
}

static void ParseCommand()
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
        LOG("[DBG] Invalid command\n");
}

static void ExecuteDebugCmd(void)
{
    if (debugCmd.params[0] == 1)
    {
        LOG("[DBG] Debug mode enabled\n");
        DebugModeEnabled = 1;
    }
    else if (debugCmd.params[0] == 0)
    {
        LOG("[DBG] Debug mode enabled\n");
        DebugModeEnabled = 1;
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
        LOG("[DBG] Srv: %d, Pos: %d", debugCmd.params[0], pos);
    }
}

