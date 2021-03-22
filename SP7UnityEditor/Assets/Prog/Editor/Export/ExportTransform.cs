using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct TransformLink
{
    public int instanceID;
    public Vector3 position;
    public Vector3 rotation;
    public Vector3 scale;
}

[System.Serializable]
public struct TransformCollection
{
    public List<TransformLink> transforms;
}

public class ExportTransform
{
    public static TransformCollection Export(string aSceneName, List<int> validInstanceIDs)
    {
        TransformCollection transformCollection = new TransformCollection();
        transformCollection.transforms = new List<TransformLink>();
        Transform[] transforms = GameObject.FindObjectsOfType<Transform>();
        foreach (Transform transform in transforms)
        {
            if (validInstanceIDs.Contains(transform.GetInstanceID()))
            {
                TransformLink link = new TransformLink();
                link.instanceID = transform.GetInstanceID();

                if(transform.GetComponentInChildren<Renderer>() != null)
                {
                    Renderer rend = transform.GetComponentInChildren<Renderer>();
                    if (rend.sharedMaterial.shader.name.Contains("Decal"))
                    {
                        link.rotation = transform.rotation.eulerAngles;
                    }
                    else 
                    { 
                        link.rotation = transform.ConvertToIronWroughtRotation();
                    }
                }
                else
                {
                    link.rotation = transform.ConvertToIronWroughtRotation();
                }
                link.position = transform.ConvertToIronWroughtPosition();
                link.scale = transform.ConvertToIronWroughtScale();
                transformCollection.transforms.Add(link);
            }        
        }
        return transformCollection;
        //Json.ExportToJson(transformCollection, aScene.name);
    }
}
