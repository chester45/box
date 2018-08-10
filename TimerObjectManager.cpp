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

uint8_t TimerObjectManager::CreateTimer(unsigned long int ms, void *param, TimerCallback_t callback, bool isSingle)
{
    uint8_t TimerIndex = GetFreeTimer();
    if(TimerIndex != INVALID_TIMER_IDX)
    {
        TimersArray[TimerIndex] = new TimerObject(ms, param, callback, isSingle);
    }
    return TimerIndex;
}

void TimerObjectManager::UpdateTimers()
{
    for (uint8_t idx = 0; idx < MAX_TIMER_OBJECTS; idx++)
    {
        if (TimersArray[idx] != nullptr)
        {
            TimersArray[idx]->Update();
        }
    }
}

uint8_t TimerObjectManager::GetFreeTimer()
{
    uint8_t TimerIndex = INVALID_TIMER_IDX;
    for (uint8_t idx = 0; idx < MAX_TIMER_OBJECTS; idx++)
    {
        if (TimersArray[idx] == nullptr)
        {
            TimerIndex = idx;
            break;              /// Break the loop sinde we found free slot for timer
        }
    }
    return TimerIndex;
}

bool TimerObjectManager::StartTimer(uint8_t TimerIndex)
{
    if (IsValidTimerIndex(TimerIndex))
    {
        TimersArray[TimerIndex]->Start();
        return true;
    }
    return false;
}

bool TimerObjectManager::StopTimer(uint8_t TimerIndex)
{
    if (IsValidTimerIndex(TimerIndex))
    {
        TimersArray[TimerIndex]->Stop();
        return true;
    }
    return false;
}

bool TimerObjectManager::DestroyTimer(uint8_t TimerIndex)
{
    if (IsValidTimerIndex(TimerIndex))
    {
        delete TimersArray[TimerIndex];
        TimersArray[TimerIndex] = nullptr;
        return true;
    }
    return false;
}

bool TimerObjectManager::IsValidTimerIndex(uint8_t Index)
{
    if (Index != INVALID_TIMER_IDX && TimersArray[Index] != nullptr)
    {
        return true;
    }
    return false;
}