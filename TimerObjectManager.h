#ifndef _TIMER_OBJECT_MANAGER_H
#define _TIMER_OBJECT_MANAGER_H

#include "stdlib.h"
#include "TimerObject.h"

#define INVALID_TIMER_IDX   255

class TimerObjectManager
{
    public:
        typedef void (*TimerCallback_t)(void*);
        static TimerObjectManager* GetManager();
        uint8_t CreateTimer(unsigned long int delayMs, void *param, TimerCallback_t callback,
                            bool isSingle);
        bool StartTimer(uint8_t timerIndex);
        bool StopTimer(uint8_t timerIndex);
        bool SetInterval(uint8_t timerIndex, uint32_t interval);
        bool DestroyTimer(uint8_t timerIndex);

        void UpdateTimers();

    private:
        static TimerObjectManager *instance;
        static const uint8_t MAX_TIMER_OBJECTS = 5;
        TimerObject *TimersArray[MAX_TIMER_OBJECTS];

        /// This is singelton class so user should not have access to contructors
        TimerObjectManager() {};
        TimerObjectManager(const TimerObjectManager&) = delete;
        TimerObjectManager & operator=(const TimerObjectManager&) = delete;
        ~TimerObjectManager(){};

        void Init();
        uint8_t GetFreeTimer();
        bool IsValidTimerIndex(uint8_t);
};

#endif