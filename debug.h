#ifndef DEBUG_H
#define DEBUG_H

#include "box.h"

typedef void (*CmdFunc_t)(void);

typedef struct {
        char cmd;
        CmdFunc_t execFunc;
        const char *h_str;
} DebugCommand_t;

typedef struct {
        char cmd;
        uint8_t validParams;
        int params[3];
} Command_t;

void InitDebugModule(Box::Box *);
bool IsDebugModeEnabled(void);
void FetchDebugCommand(void);

#endif