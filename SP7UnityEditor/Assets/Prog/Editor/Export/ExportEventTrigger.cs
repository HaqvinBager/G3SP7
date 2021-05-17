using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct EventData
{
    public Transform instanceID;
    public string gameEvent;
    public EEventFilter eventFilter;
    public int audioIndex;
    public bool triggerOnce;
}

[System.Serializable]
public struct EventCollection
{
    public List<EventData> triggerEvents;
}

public class ExportEventTrigger 
{

    public static EventCollection Export(string aScene)
    {
        EventCollection collection = new EventCollection();
        collection.triggerEvents = new List<EventData>();

        IronEvent[] ironEvents = GameObject.FindObjectsOfType<IronEvent>();
        foreach (IronEvent ironEvent in ironEvents)
        {
            collection.triggerEvents.Add(new EventData {
                instanceID = ironEvent.transform,
                gameEvent = ironEvent.eventObject.name,
                eventFilter = ironEvent.eventFilter,
                audioIndex = ironEvent.audioIndex,
                triggerOnce = ironEvent.triggerOnce
            }); ;
        }

        return collection;
    }
   
}
