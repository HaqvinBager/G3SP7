#include "stdafx.h"
#include "GraphNodeCollisionManager.h"
//#include "Scene.h"
#include <algorithm>

CGraphNodeCollisionManager* CGraphNodeCollisionManager::ourInstance = nullptr;

void CGraphNodeCollisionManager::AddCollision
(callback_function_collision anEnterCallback
	, callback_function_collision aStayCallback
	, callback_function_collision anExitCallback
	, std::string aListenType
	, int aListenID
	, int aTriggerID
	, int aUID
	, bool aDoOnce
)
{
	SCollision collision = {};
	collision.myEnterCallback = anEnterCallback;
	collision.myStayCallback = aStayCallback;
	collision.myExitCallback = anExitCallback;
	collision.myListenType = aListenType;
	collision.myListenObjectID = aListenID;
	collision.myTriggerObjectID = aTriggerID;
	collision.myUID = aUID;
	collision.myDoOnce = aDoOnce;
	myCollisions.emplace_back(collision);
}

void CGraphNodeCollisionManager::Update()
{
	bool colliding = false;
	//int anID = 0;
	std::vector<int> indicesOfCollisionsToRemove;
	for (unsigned int i = 0; i < myCollisions.size(); ++i)
	{
		SCollision& collisionCheck = myCollisions[i];

		//if (collisionCheck.myListenType.empty()) {
		//	colliding = CScene::GetActiveScene()->Collided(
		//		collisionCheck.myListenObjectID,
		//		collisionCheck.myTriggerObjectID
		//	);
		//}
		//else {
		//	colliding = CScene::GetActiveScene()->Collided(
		//		collisionCheck.myListenType,
		//		collisionCheck.myTriggerObjectID,
		//		anID
		//	);
		//	collisionCheck.myListenObjectID = anID;
		//}

		if (colliding && !collisionCheck.myCollided)
		{
			collisionCheck.myCollided = true;
			collisionCheck.myEnterCallback(collisionCheck.myListenObjectID);

			if (collisionCheck.myDoOnce)
			{
				indicesOfCollisionsToRemove.emplace_back(i);
			}
		}
		else if (colliding && collisionCheck.myCollided && !collisionCheck.myDoOnce)
		{
			collisionCheck.myStayCallback(collisionCheck.myListenObjectID);
		}
		else if (!colliding && collisionCheck.myCollided)
		{
			collisionCheck.myCollided = false;
			collisionCheck.myExitCallback(collisionCheck.myListenObjectID);
		}
	}

	std::sort(indicesOfCollisionsToRemove.begin(), indicesOfCollisionsToRemove.end(), std::greater());
	for (auto& index : indicesOfCollisionsToRemove)
	{
		std::swap(myCollisions[index], myCollisions.back());
		myCollisions.pop_back();
	}
}

int CGraphNodeCollisionManager::RequestUID()
{
	return myUIDCounter++;
}

void CGraphNodeCollisionManager::RemoveColliderWithID(int aUID)
{
	for (unsigned int i = 0; i < myCollisions.size(); ++i)
	{
		if (myCollisions[i].myUID == aUID)
		{
			std::swap(myCollisions[i], myCollisions.back());
			myCollisions.pop_back();
			return;
		}
	}
}
