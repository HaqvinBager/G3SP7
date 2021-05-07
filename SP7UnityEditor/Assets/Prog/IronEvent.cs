using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum IronEventType
{
    //SoundEffectEvent,
    GameEvent,
    //VFXEvent
}

public enum EEventFilter // Needs to be 1:1 in C++
{
    PlayerOnly,
    EnemyOnly,
    ObjectOnly,
    Any
}

public class IronEvent : MonoBehaviour
{
    public IronEventType eventType;
    public GameEvent eventObject;
    public EEventFilter eventFilter = EEventFilter.Any;
}