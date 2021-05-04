using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct HealthPickupLink
{
    public int instanceID;
    public float healthPickupAmount;
}

[System.Serializable]
public struct HealthPickUpCollection
{
    public List<HealthPickupLink> healthPickups;
}

public class HealthPickupExporter
{
    public static HealthPickUpCollection Export(string aSceneName)
    {

        HealthPickUpCollection collection = new HealthPickUpCollection();
        collection.healthPickups = new List<HealthPickupLink>();

        HealthPickup[] allHealthPickups = GameObject.FindObjectsOfType<HealthPickup>();
        foreach (HealthPickup health in allHealthPickups)
        {
            HealthPickupLink healthPickupLink;
            healthPickupLink.instanceID = health.transform.GetInstanceID();
            healthPickupLink.healthPickupAmount = health.healthPickupAmount;
            collection.healthPickups.Add(healthPickupLink);
        }

        return collection;
    }

}
