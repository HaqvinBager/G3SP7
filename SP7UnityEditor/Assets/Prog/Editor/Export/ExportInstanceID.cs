using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
struct InstanceIDCollection
{
    public string sceneName;
    public List<int> Ids; 
}

public class ExportInstanceID 
{
    public static void Export(Scene aScene)
    {
        Transform[] transforms = GameObject.FindObjectsOfType<Transform>();
        InstanceIDCollection sceneIDCollection = new InstanceIDCollection();
        sceneIDCollection.Ids = new List<int>();
        foreach(Transform transform in transforms)
        {
            if (Json.TryIsValidExport(transform, out GameObject prefabParent))
            {
                if (sceneIDCollection.Ids.Exists(e => e == prefabParent.transform.GetInstanceID()))
                    continue;

                sceneIDCollection.Ids.Add(prefabParent.transform.GetInstanceID());
            }    
        }

        sceneIDCollection.sceneName = aScene.name;
        Json.ExportToJson(sceneIDCollection, aScene.name);
    }
}
