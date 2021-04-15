using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct STransform
{
    public Vector3 position;
    public Vector3 rotation;
    public Vector3 scale;
}

[System.Serializable]
public class InstancedModel
{
    public int assetID;
    public List<STransform> transforms;
}

[System.Serializable]
public class InstanceModelCollection
{
    public List<InstancedModel> instancedModels;
}

public class ExportInstancedModel
{
    public static InstanceModelCollection Export(string aSceneName)
    {
        InstanceModelCollection collection = new InstanceModelCollection();
        collection.instancedModels = new List<InstancedModel>();

        MeshFilter[] meshFilters = GameObject.FindObjectsOfType<MeshFilter>();
        foreach (var meshFilter in meshFilters)
        {
            if (Json.TryIsValidExport(meshFilter, out GameObject prefabParent, true))
            {
                //if (prefabParent.GetComponent<Collider>() != null)
                //    continue;

                GameObject asset = PrefabUtility.GetCorrespondingObjectFromOriginalSource(meshFilter).gameObject;
                if (asset == null)
                    continue;


                int assetID = asset.transform.GetInstanceID();
                InstancedModel instancedModel = collection.instancedModels.Find(e => e.assetID == assetID);
                if (instancedModel == null)
                {
                    instancedModel = new InstancedModel();
                    instancedModel.assetID = assetID;
                    instancedModel.transforms = new List<STransform>();
                    collection.instancedModels.Add(instancedModel);
                }

                STransform transform = new STransform();
                transform.position = prefabParent.transform.ConvertToIronWroughtPosition();
                transform.rotation = prefabParent.transform.ConvertToIronWroughtRotation();
                transform.scale = prefabParent.transform.ConvertToIronWroughtScale();

                instancedModel.transforms.Add(transform);
            }

        }
        //Json.ExportToJson(collection, aScene.name);
        return collection;
    }
}
