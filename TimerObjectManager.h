#include "stdlib.h"
#include "Arduino.h"
#include "TimerObject.h"

class TimerObjectManager
{
    public:
        static const uint8_t INVALID_TIMER_IDX = 255;
        static TimerObjectManager* GetManager();
        uint8_t CreateTimer();
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
};