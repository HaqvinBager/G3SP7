using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;


[System.Serializable]
public class ModelLink
{
    public int instanceID;
    public int assetID;
}

[System.Serializable]
public class ModelCollection
{
    public List<ModelLink> modelLinks;
}

public class ExportModel
{
    public static ModelCollection Export(string aSceneName, List<int> validInstanceIds)
    {
        ModelCollection modelCollection = new ModelCollection();
        modelCollection.modelLinks = new List<ModelLink>();

        Renderer[] allrenderers = GameObject.FindObjectsOfType<Renderer>();

        foreach(Renderer renderer in allrenderers)
        {
            if (renderer.sharedMaterial.shader.name.Contains("Decal"))
                continue;

            if (renderer.TryGetComponent(out PolybrushFBX polyBrushFbx))
            {
                string assetPath = AssetDatabase.GUIDToAssetPath(polyBrushFbx.originalFBXGUID);
                GameObject modelAsset = AssetDatabase.LoadAssetAtPath<GameObject>(assetPath);

                ModelLink link = new ModelLink();
                link.assetID = modelAsset.transform.GetInstanceID();
                link.instanceID = renderer.transform.parent.GetInstanceID();

                if(!modelCollection.modelLinks.Exists( e => e.instanceID == link.instanceID))
                    modelCollection.modelLinks.Add(link);

                continue;
            } 
            else if(Json.TryIsValidExport(renderer, out GameObject prefabParent))
            {
                if (renderer.TryGetComponent(out MeshFilter meshFilter))
                {
                    GameObject modelAsset = PrefabUtility.GetCorrespondingObjectFromOriginalSource(meshFilter).gameObject;
                    
                    if (modelAsset == null)
                        continue;

                    if (modelAsset.transform.parent != null)
                        modelAsset = modelAsset.transform.parent.gameObject;

                    if (prefabParent.name.Contains("BP_"))
                    {

                        Renderer[] childRenderers = prefabParent.GetComponentsInChildren<Renderer>(); 
                        foreach(Renderer childRenderer in childRenderers)
                        {
                            if(GetNearestPrefabInstance(childRenderer, out Transform validPrefabParent, validInstanceIds))
                            {
                                ModelLink link = new ModelLink();
                                link.assetID = modelAsset.transform.GetInstanceID();
                                link.instanceID = validPrefabParent.transform.GetInstanceID();

                                if(!modelCollection.modelLinks.Exists(e => e.instanceID == link.instanceID))
                                {
                                    modelCollection.modelLinks.Add(link);
                                }
                            }
                        }
                    }
                    else
                    {
                        ModelLink link = new ModelLink();
                        link.assetID = modelAsset.transform.GetInstanceID();
                        link.instanceID = prefabParent.transform.GetInstanceID();
                        modelCollection.modelLinks.Add(link);
                    }
                }
            }           
        }
        // Json.ExportToJson(fbxLinks, aScene.name);
        return modelCollection;
    }

    public static bool GetNearestPrefabInstance<T>(T aInstance, out Transform outNearestPrefabInstance, List<int> validInstanceIDs) where T : Component
    {
        outNearestPrefabInstance = null;

        T source = PrefabUtility.GetCorrespondingObjectFromOriginalSource(aInstance);
        if (source != null)
        {
            outNearestPrefabInstance = GetNearestValidParent(aInstance.transform, validInstanceIDs);
        }
        return outNearestPrefabInstance != null;
    }

    private static Transform GetNearestValidParent(Transform aChild, List<int> validInstanceIDs)
    {
        if (aChild.parent != null)
        {
            if (validInstanceIDs.Contains(aChild.parent.GetInstanceID()))
            {
                return aChild.parent;
            }
            else
            {
                return GetNearestValidParent(aChild.parent, validInstanceIDs);
            }
        }

        return null;
    }


}
