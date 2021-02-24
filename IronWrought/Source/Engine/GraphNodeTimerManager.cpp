#include "stdafx.h"
#include "GraphNodeTimerManager.h"
#include <algorithm>

CGraphNodeTimerManager* CGraphNodeTimerManager::ourInstance = nullptr;
void CGraphNodeTimerManager::AddTimer(callback_function_timer anInstance, float aDuration, int aUID, bool aShouldLoop)
{
    STimer timer = {};
    timer.myCallback = anInstance;
    timer.myDuration = aDuration;
    timer.myFinishedTime = /*myTimer.GetTotalSeconds()*/ + aDuration;
    timer.myUID = aUID;
    timer.myShouldLoop = aShouldLoop;
    myTimers.emplace_back(timer);
}

void CGraphNodeTimerManager::Update()
{
    std::vector<int> indicesOfTimersToRemove;
    //myTimer.Tick([]() {});
    
    for (unsigned int i = 0; i < myTimers.size(); ++i)
    {
        //STimer& timer = myTimers[i];

        //if (timer.myFinishedTime < myTimer.GetTotalSeconds())
        //{
        //    timer.myCallback();
        //    
        //    if (timer.myShouldLoop)
        //    {
        //        timer.myFinishedTime = myTimer.GetTotalSeconds() + timer.myDuration;
        //    } 
        //    else 
        //    {
        //        indicesOfTimersToRemove.emplace_back(i);
        //    }
        //}
    }

    std::sort(indicesOfTimersToRemove.begin(), indicesOfTimersToRemove.end(), std::greater());
    for (auto& index : indicesOfTimersToRemove)
    {
        std::swap(myTimers[index], myTimers.back());
        myTimers.pop_back();
    }
}

int CGraphNodeTimerManager::RequestUID()
{
    return myUIDCounter++;
}

void CGraphNodeTimerManager::RemoveTimerWithID(int aUID)
{
    for (unsigned int i = 0; i < myTimers.size(); ++i)
    {
        if (myTimers[i].myUID == aUID)
        {
            std::swap(myTimers[i], myTimers.back());
            myTimers.pop_back();
            return;
        }
    }
}
