#include "stdlib.h"
#include "TimerObject.h"

class TimerObjectManager
{
    public:
        typedef void (*TimerCallback_t)(void*);
        static const uint8_t INVALID_TIMER_IDX = 255;
        static TimerObjectManager* GetManager();
        uint8_t CreateTimer(unsigned long int DelayMs, void *Param, TimerCallback_t Callback, bool IsSingle);
        bool StartTimer(uint8_t TimerIndex);
        bool StopTimer(uint8_t TimerIndex);
        bool DestroyTimer(uint8_t TimerIndex);

        void UpdateTimers();

    private:
        static TimerObjectManager *Instance;

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