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
    public static ColliderCollection Export(string aSceneName, List<int> validInstanceIDs)
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

                if (collider.GetType() == typeof(BoxCollider))
                {
                    BoxCollider boxCollider = collider as BoxCollider;
                    link.colliderType = 1;
                    link.positionOffest = boxCollider.center;
                    link.boxSize = boxCollider.size;
                }
                else if (collider.GetType() == typeof(SphereCollider))
                {
                    SphereCollider sphereCollider = collider as SphereCollider;
                    link.colliderType = 2;
                    link.positionOffest = sphereCollider.center;
                    link.sphereRadius = sphereCollider.radius;
                }
                else if (collider.GetType() == typeof(CapsuleCollider))
                {
                    CapsuleCollider capsuleCollider = collider as CapsuleCollider;
                    link.colliderType = 3;
                    link.positionOffest = capsuleCollider.center;
                    link.capsuleHeight = capsuleCollider.height;
                    link.capsuleRadius = capsuleCollider.radius;
                }
                else
                {
                    Debug.LogError("We don't accept This type of Collider: " + collider.GetType(), collider);
                }
                colliderCollection.colliders.Add(link);
            }
        }
        return colliderCollection;
    }
}
