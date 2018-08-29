#include "TimerObjectManager.h"
#include "log.h"

DEFINE_DEBUG_TAG("[TOM]");

TimerObjectManager* TimerObjectManager::Instance = NULL;

TimerObjectManager* TimerObjectManager::GetManager()
{
    if (Instance == NULL)
    {
        Instance = new TimerObjectManager();
        Instance->Init();
    }
    return Instance;
}

void TimerObjectManager::Init()
{
    for (uint8_t idx = 0; idx < MAX_TIMER_OBJECTS; idx++)
    {
        TimersArray[idx] = NULL;
    }
}

uint8_t TimerObjectManager::CreateTimer(unsigned long int ms, void *param, TimerCallback_t callback, bool isSingle)
{
    uint8_t TimerIndex = GetFreeTimer();
    if(TimerIndex != INVALID_TIMER_IDX)
    {
        TimersArray[TimerIndex] = new TimerObject(ms, param, callback, isSingle);
        LOG("Create timer with index: %d\n", TimerIndex);
    }
    else
    {
        LOG("Failed to create timer object\n");
    }
    return TimerIndex;
}

void TimerObjectManager::UpdateTimers()
{
    for (uint8_t idx = 0; idx < MAX_TIMER_OBJECTS; idx++)
    {
        if (TimersArray[idx] != NULL)
        {
            TimersArray[idx]->Update();
        }
    }
}

uint8_t TimerObjectManager::GetFreeTimer()
{
    uint8_t TimerIndex = INVALID_TIMER_IDX;
    uint8_t idx = 0;
    for (idx = 0; idx < MAX_TIMER_OBJECTS; idx++)
    {
        LOG("Idx: %d, val: %d\n", idx, TimersArray[idx]);
        if (TimersArray[idx] == NULL)
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
        TimersArray[TimerIndex] = NULL;
        LOG("Destroy timer with index: %d\n", TimerIndex);
        return true;
    }
    return false;
}

bool TimerObjectManager::IsValidTimerIndex(uint8_t Index)
{
    if (Index != INVALID_TIMER_IDX && TimersArray[Index] != NULL)
    {
        return true;
    }
    return false;
}

bool TimerObjectManager::SetInterval(uint8_t TimerIndex, uint32_t Interval)
{
    if (IsValidTimerIndex(TimerIndex))
    {
        TimersArray[TimerIndex]->setInterval(Interval);
        return true;
    }
    return false;
}