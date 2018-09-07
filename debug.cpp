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
static void ExecuteSetBrightnessCmd(void);
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

#define TEMP_BUF_SIZE           (80)

void (*ResetFunc)(void) = 0;

static const char h_help[] PROGMEM = "Print help: h";
static const char d_help[] PROGMEM = "Enable debug mode:  d [0/1]";
static const char s_help[] PROGMEM = "Set servo position: s [srv_num] [pos] [speed]";
static const char g_help[] PROGMEM = "Get servo postiion: g [srv_num]";
static const char b_help[] PROGMEM = "Set LED pwm value:  b [value]";
static const char r_help[] PROGMEM = "Reset the device:   r [1]";

static const DebugCommand_t debugCmdTable[] = {
        {'h', ExecuteHelpCmd,           h_help},
        {'d', ExecuteDebugCmd,          d_help},
        {'s', ExecuteMoveServoCmd,      s_help},
        {'g', ExecuteGetServoPosCmd,    g_help},
        {'g', ExecuteSetBrightnessCmd,  b_help},
        {'r', ExecuteResetCmd,          r_help},
        // must always be last
        {' ', NULL, NULL}
};

/************************************************************************
* Public functions
************************************************************************/

void InitDebugModule(Box::Box *box)
{
        if (!box) {
                LOG_PM("Failed to init debug module\n");
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
                LOG_PM("Invalid command\n");
}

static void ExecuteHelpCmd()
{
        uint8_t idx = 0;
        char help[100];
        while (debugCmdTable[idx].cmd != ' ') {
                LOG_BASIC("\t%c", debugCmdTable[idx].cmd);
                if (debugCmdTable[idx].h_str) {
                        size_t h_len = strlen_P(debugCmdTable[idx].h_str);
                        if (h_len >= 100) {
                                LOG_BASIC_PM("help string to long!");
                        } else {
                                strcpy_P(help, debugCmdTable[idx].h_str);
                                LOG_BASIC(" %s", help);
                        }
                } else {
                        LOG_BASIC_PM(" no help available");
                }
                LOG_BASIC_PM("\n");
                idx++;
        }
}

static void ExecuteDebugCmd(void)
{
        if (CMD_PARAM_1 == 1) {
                LOG_PM("Debug mode enabled\n");
                DebugModeEnabled = 1;
        } else if (CMD_PARAM_1 == 0) {
                LOG_PM("Debug mode disabled\n");
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

static void ExecuteSetBrightnessCmd()
{
        // box_p->DebugSetBrightness(CMD_PARAM_1);
}

static void ExecuteResetCmd()
{
        if (CMD_PARAM_1 != 1) {
                LogInvalidParams();
                return;
        }
        LOG_PM("Resetting platform ... \n");
        // watchdog does not work with nano - some bootloader issues
        //wdt_enable(WDTO_1S);
        delay(3000);
        ResetFunc();
        LOG_PM("Fail to reset ! \n");
}
