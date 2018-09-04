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

#define CMD                     debugCmd.cmd
#define CMD_VALID_PARAMS        debugCmd.validParams
#define CMD_PARAM_1             debugCmd.params[0]
#define CMD_PARAM_2             debugCmd.params[1]
#define CMD_PARAM_3             debugCmd.params[2]

void (*ResetFunc)(void) = 0;

DebugCommand_t debugCmdTable [] = {
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
        if (!box) {
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
        while (Serial.available()) {
                // get the new byte:
                char inChar = (char)Serial.read();
                if (inChar == '\n') {
                        ParseCommand();
                        ResetCommandInfo();
                } else {
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
        CMD = ' ';
        CMD_PARAM_1 = 0;
        CMD_PARAM_2 = 0;
        CMD_PARAM_3 = 0;
        CMD_VALID_PARAMS = 0;
}

static void LogInvalidParams()
{
        LOG("Invalid cmd params: %d %d %d\n", CMD_PARAM_1, CMD_PARAM_2, CMD_PARAM_3);
}

static void ParseCommand()
{
        if (cmdString.length() < 3) {
                LOG("Wrong length: %d\n", cmdString.length());
                return;
        }

        CMD = cmdString[0];
        int paramStartIdx = 2;

        for (uint8_t i = 0; i < 3; i ++) {
                int paramEndIdx = cmdString.indexOf(" ", paramStartIdx);
                if (paramEndIdx == -1) {   // This is last parameter
                        CMD_VALID_PARAMS++;
                        debugCmd.params[i] = cmdString.substring(paramStartIdx).toInt();
                        break;
                } else {
                        CMD_VALID_PARAMS++;
                        debugCmd.params[i] = cmdString.substring(paramStartIdx, paramEndIdx).toInt();
                        paramStartIdx = paramEndIdx + 1;
                }
        }
        ExecuteCommand();
}

/*
static void PrintCommand()
{
    LOG("Command: %c, %d, %d, %d, %d\n", CMD,
                                         CMD_VALID_PARAMS,
                                         CMD_PARAM_1,
                                         CMD_PARAM_2,
                                         CMD_PARAM_3);
}
*/

static void ExecuteCommand()
{
        uint8_t idx = 0;
        bool cmdFound = false;

        while (debugCmdTable[idx].cmd != ' ') {
                if (CMD == debugCmdTable[idx].cmd) {
                        if (debugCmdTable[idx].execFunc != NULL) {
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
        while (debugCmdTable[idx].cmd != ' ') {
                LOG_BASIC("\t%c", debugCmdTable[idx].cmd);
                if (debugCmdTable[idx].h_str) {
                        LOG_BASIC(" %s", debugCmdTable[idx].h_str);
                } else {
                        LOG_BASIC(" no help available");
                }
                LOG_BASIC("\n");
                idx++;
        }
}

static void ExecuteDebugCmd(void)
{
        if (CMD_PARAM_1 == 1) {
                LOG("Debug mode enabled\n");
                DebugModeEnabled = 1;
        } else if (CMD_PARAM_1 == 0) {
                LOG("Debug mode disabled\n");
                DebugModeEnabled = 0;
        } else {
                LogInvalidParams();
        }
}

static void ExecuteMoveServoCmd()
{
        if (CMD_PARAM_1 > 1 || CMD_PARAM_2 > 180 || CMD_PARAM_3 > SLOW) {
                LogInvalidParams();
        } else {
                box_p->DebugMoveServo(CMD_PARAM_1, CMD_PARAM_2, (MoveSpeed_t)CMD_PARAM_3);
        }
}

static void ExecuteGetServoPosCmd()
{
        if (CMD_PARAM_1 > 1) {
                LogInvalidParams();
        } else {
                uint8_t pos = box_p->DebugGetServoPosition(CMD_PARAM_1);
                LOG("Srv: %d, Pos: %d\n", CMD_PARAM_1, pos);
        }
}

static void ExecuteResetCmd()
{
        if (CMD_PARAM_1 != 1) {
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
