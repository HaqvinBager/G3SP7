using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct EventData
{
    public Transform instanceID;
    public List<EventType> eventTypes;
}

[System.Serializable]
public struct EventCollection
{
    public List<EventData> events;
}

public class ExportEventTrigger 
{

    public static EventCollection Export(string aScene)
    {
        EventCollection collection = new EventCollection();
        collection.events = new List<EventData>();

        return collection;
    }
   
}
