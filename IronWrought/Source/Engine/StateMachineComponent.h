#pragma once
#include "Component.h"
#include <map>


//template<class T>
struct SState {
	std::string myName;
	//void* myObject;
};


//template<class T>
class CStateMachineComponent : public CComponent
{
public:
	void AddState(SState* aState);

private:
	SState* myCurrentState;
	std::vector<SState*> myStates;

public:
	CStateMachineComponent(CGameObject& aParent);
	~CStateMachineComponent() override;
public:
	void Awake() override;
	void Start() override;
	void Update() override;



//#ifdef _IMGUI
public:
	struct SerializedObject {
		SerializedObject(std::vector<SState*>& someStates) : myStates(someStates) {

		}
		std::vector<SState*>& myStates;
	};

	SerializedObject GetData() {
		return SerializedObject(myStates);
	}
//#endif
};

