#pragma once
#include <functional>
#include "Heap.h"

using callback_function_timer = std::function<void()>;
class CGraphNodeTimerManager
{
public:
    static void Create() { 
        ourInstance = new CGraphNodeTimerManager();
        ourInstance->myUIDCounter = -1000;
    }
    static CGraphNodeTimerManager* Get() { return ourInstance; }
private:
    static CGraphNodeTimerManager* ourInstance;

public:
    void AddTimer(callback_function_timer anInstance, float aDuration, int aUID, bool aShouldLoop);
    void Update();

    int RequestUID();
    void RemoveTimerWithID(int aUID);

private:
    struct STimer
    {
        callback_function_timer myCallback;
        float myDuration;
        float myFinishedTime;
        int myUID;
        bool myShouldLoop = false;
    };

    std::vector<STimer> myTimers;
    int myUIDCounter;
};