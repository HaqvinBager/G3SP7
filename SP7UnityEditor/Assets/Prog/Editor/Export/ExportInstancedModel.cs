using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
struct InstancedModel
{
    public int modelIndex;
    public List<int> instanceIDs;
}

[System.Serializable]
struct InstanceModelCollection
{
    public List<InstancedModel> instancedModels;
}


public class ExportInstancedModel 
{
    public static void Export(Scene aScene)
    {
        MeshRenderer[] renderers = GameObject.FindObjectsOfType<MeshRenderer>();
        foreach(var renderer in renderers)
        {
            if(Json.TryIsValidExport(renderer, out GameObject prefabParent))
            {
                if (prefabParent.isStatic)
                {

                }
            }
        }
    }

}
