#pragma once

using callback_function_collision = std::function<void(int)>;
class CGraphNodeCollisionManager
{
public:
    static void Create() {
        ourInstance = new CGraphNodeCollisionManager();
        ourInstance->myUIDCounter = -1000;
    }
    static CGraphNodeCollisionManager* Get() {
        return ourInstance;
    }

    void AddCollision(callback_function_collision anEnterCallback
        , callback_function_collision aStayCallback
        , callback_function_collision anExitCallback
        , std::string aListenType
        , int aListenID
        , int aTriggerID
        , int aUID
        , bool aDoOnce
    );
    void Update();

    int RequestUID();
    void RemoveColliderWithID(int aUID);

private:
    struct SCollision
    {
        callback_function_collision myEnterCallback;
        callback_function_collision myStayCallback;
        callback_function_collision myExitCallback;
        std::string myListenType;
        int myListenObjectID;
        int myTriggerObjectID;
        bool myDoOnce = false;
        bool myCollided = false;
        int myUID;
    };

    std::vector<SCollision> myCollisions;
    static CGraphNodeCollisionManager* ourInstance;
    int myUIDCounter;
};