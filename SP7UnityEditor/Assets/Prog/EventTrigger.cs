using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum Event
{
    LoadLevel,
    OpenDoor,
    EnterLadder,
    StartSoundFX,
    StartDialogue
}

public class EventTrigger : MonoBehaviour
{
    public Event anEvent;
}

