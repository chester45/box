#include "TimerObjectManager.h"

TimerObjectManager* TimerObjectManager::Instance = nullptr;

TimerObjectManager* TimerObjectManager::GetManager()
{
    if (Instance == nullptr)
    {
        Instance = new TimerObjectManager();
    }
    return Instance;
}

void TimerObjectManager::Init()
{
    for (uint8_t idx = 0; idx < MAX_TIMER_OBJECTS; idx++)
    {
        TimersArray[idx] = nullptr;
    }
}

uint8_t TimerObjectManager::CreateTimer()
{
    return INVALID_TIMER_IDX;
}