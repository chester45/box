#include "TimerObjectManager.h"
#include "log.h"

DEFINE_DEBUG_TAG("[TOM]");

TimerObjectManager* TimerObjectManager::instance = NULL;

TimerObjectManager* TimerObjectManager::GetManager()
{
        if (instance == NULL) {
            instance = new TimerObjectManager();
            instance->Init();
        }
        return instance;
}

void TimerObjectManager::Init()
{
        for (uint8_t idx = 0; idx < MAX_TIMER_OBJECTS; idx++) {
            TimersArray[idx] = NULL;
        }
}

uint8_t TimerObjectManager::CreateTimer(unsigned long int ms, void *param,
                                        TimerCallback_t callback,
                                        bool isSingle) {

        uint8_t timerIndex = GetFreeTimer();

        if (timerIndex != INVALID_TIMER_IDX) {
                TimersArray[timerIndex] = new TimerObject(ms, param, callback, isSingle);
                LOG("Create timer with index: %d\n", timerIndex);
        } else {
                LOG("Failed to create timer object\n");
        }
        return timerIndex;
}

void TimerObjectManager::UpdateTimers()
{
        for (uint8_t idx = 0; idx < MAX_TIMER_OBJECTS; idx++) {
                if (TimersArray[idx] != NULL) {
                    TimersArray[idx]->Update();
                }
        }
}

uint8_t TimerObjectManager::GetFreeTimer()
{
        uint8_t timerIndex = INVALID_TIMER_IDX;
        uint8_t idx = 0;

        for (idx = 0; idx < MAX_TIMER_OBJECTS; idx++) {
                LOG("Idx: %d, val: %d\n", idx, TimersArray[idx]);
                if (TimersArray[idx] == NULL) {
                        timerIndex = idx;
                        break;  /// Break the loop sinde we found free slot for timer
                }
        }
        return timerIndex;
}

bool TimerObjectManager::StartTimer(uint8_t timerIndex)
{
        if (IsValidTimerIndex(timerIndex)) {
                TimersArray[timerIndex]->Start();
                return true;
        }
        return false;
}

bool TimerObjectManager::StopTimer(uint8_t timerIndex)
{
        if (IsValidTimerIndex(timerIndex)) {
                TimersArray[timerIndex]->Stop();
                return true;
        }
        return false;
}

bool TimerObjectManager::DestroyTimer(uint8_t timerIndex)
{
        if (IsValidTimerIndex(timerIndex)) {
                delete TimersArray[timerIndex];
                TimersArray[timerIndex] = NULL;
                LOG("Destroy timer with index: %d\n", timerIndex);
                return true;
        }
        return false;
}

bool TimerObjectManager::IsValidTimerIndex(uint8_t index)
{
        if (index != INVALID_TIMER_IDX && TimersArray[index] != NULL) {
                return true;
        }
        return false;
}

bool TimerObjectManager::SetInterval(uint8_t timerIndex, uint32_t interval)
{
        if (IsValidTimerIndex(timerIndex)) {
                TimersArray[timerIndex]->setInterval(interval);
                return true;
        }
        return false;
}