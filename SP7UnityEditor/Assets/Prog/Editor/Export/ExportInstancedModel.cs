using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
class InstancedModel
{
    public int assetID;
    public List<int> instanceIDs;
}

[System.Serializable]
class InstanceModelCollection
{
    public List<InstancedModel> instancedModels;
}


public class ExportInstancedModel 
{
    public static void Export(Scene aScene)
    {
        InstanceModelCollection collection = new InstanceModelCollection();
        collection.instancedModels = new List<InstancedModel>();

        MeshFilter[] meshFilters = GameObject.FindObjectsOfType<MeshFilter>();
        foreach(var meshFilter in meshFilters)
        {
            if(Json.TryIsValidExport(meshFilter, out GameObject prefabParent))
            {
                if (prefabParent.isStatic)
                {
                    GameObject asset = PrefabUtility.GetCorrespondingObjectFromOriginalSource(meshFilter).gameObject;
                    if (asset == null)
                        continue;

                    int assetID = asset.transform.GetInstanceID();
                    InstancedModel instancedModel = collection.instancedModels.Find(e => e.assetID == assetID);
                    if(instancedModel == null)
                    {
                        instancedModel = new InstancedModel();
                        instancedModel.assetID = assetID;
                        instancedModel.instanceIDs = new List<int>();
                    }

                    instancedModel.instanceIDs.Add(prefabParent.transform.GetInstanceID());
                }
            }
        }
        Json.ExportToJson(collection, aScene.name);
    }
}
