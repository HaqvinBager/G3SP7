using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct InstanceIDCollection
{
    public string sceneName;
    public List<int> Ids; 
}

public class ExportInstanceID 
{
    public static InstanceIDCollection Export(string aSceneName)
    {
        Transform[] transforms = GameObject.FindObjectsOfType<Transform>();
        InstanceIDCollection sceneIDCollection = new InstanceIDCollection();
        sceneIDCollection.Ids = new List<int>();
       
        foreach(Transform transform in transforms)
        {
            //Denna funktion tar in det objekt vi loopar igenom just nu, kan t.ex vara en "pointLight" från en GameObject.FindObjectsOfType<Light>();
            //out GameObject prefabParent == Parenten (prefab-parent) som denna tillhör! Alltså ska pointlights ligga som children i en tom Prefab! 
            //(Som med alla andra objekt!)
            if(transform.TryGetComponent(out Collider collider))
            {
                if (sceneIDCollection.Ids.Exists(e => e == transform.GetInstanceID()))
                    continue;
                sceneIDCollection.Ids.Add(transform.GetInstanceID());

                if (transform.name.Contains("BP_"))
                {
                    foreach (Transform childTransform in transform)
                    {
                        sceneIDCollection.Ids.Add(childTransform.GetInstanceID());
                    }
                }
            }   
            else if (Json.TryIsValidExport(transform, out GameObject prefabParent))
            {
                //Kollar bara om vi redan har lagt till denna id (Eftersom vi kollar Parent & Child objekt,
                //Men bara vill spara Parent Object ID:et
                if (sceneIDCollection.Ids.Exists(e => e == prefabParent.transform.GetInstanceID()))
                    continue;

                sceneIDCollection.Ids.Add(prefabParent.transform.GetInstanceID());

                if (prefabParent.name.Contains("BP_"))
                {
                    foreach(Transform childTransform in prefabParent.transform)
                    {
                        sceneIDCollection.Ids.Add(childTransform.GetInstanceID());
                    }
                }
            }
        }

        sceneIDCollection.sceneName = aSceneName;
        return sceneIDCollection;
    }
}
