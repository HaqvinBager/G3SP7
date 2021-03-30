using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct ColliderLink
{
    public int instanceID;
    public Vector3 positionOffest;
    public int colliderType;
    public Vector3 boxSize;
    public float sphereRadius;
    public float capsuleHeight;
    public float capsuleRadius;

}
// 1 - BoxCollider
// 2 - SphereCollider
// 3 - CapsuleCollider

[System.Serializable]
public struct ColliderCollection
{
    public List<ColliderLink> colliders;
}

public class ExportCollider : MonoBehaviour
{
    public static void Export(Scene aScene, List<int> validInstanceIDs)
    {
        ColliderCollection colliderCollection = new ColliderCollection();
        colliderCollection.colliders = new List<ColliderLink>();
        Collider[] colliderss = GameObject.FindObjectsOfType<Collider>();
        foreach (Collider collider in colliderss)
        {
            if (validInstanceIDs.Contains(collider.transform.GetInstanceID()))
            {
                ColliderLink link = new ColliderLink();
                link.instanceID = collider.transform.GetInstanceID();

                if (collider.GetComponent<BoxCollider>() != null)
                {
                    link.positionOffest = collider.GetComponent<BoxCollider>().center;
                    link.colliderType = 1;
                    link.boxSize = collider.GetComponent<BoxCollider>().size;
                } else if(collider.GetComponent<SphereCollider>() != null)
                {
                    link.positionOffest = collider.GetComponent<SphereCollider>().center;
                    link.colliderType = 2;
                    link.sphereRadius = collider.GetComponent<SphereCollider>().radius;
                } else if (collider.GetComponent<CapsuleCollider>() != null)
                {
                    link.positionOffest = collider.GetComponent<CapsuleCollider>().center;
                    link.colliderType = 3;
                    link.capsuleHeight = collider.GetComponent<CapsuleCollider>().height;
                    link.capsuleRadius = collider.GetComponent<CapsuleCollider>().radius;
                }
                colliderCollection.colliders.Add(link);
            }
        }
        Json.ExportToJson(colliderCollection, aScene.name);
    }
}
