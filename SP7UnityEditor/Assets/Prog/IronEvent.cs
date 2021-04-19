using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum IronEventType
{
    //SoundEffectEvent,
    GameEvent,
    //VFXEvent
}

public class IronEvent : MonoBehaviour
{
    public IronEventType eventType;
    public GameEvent eventObject;
}