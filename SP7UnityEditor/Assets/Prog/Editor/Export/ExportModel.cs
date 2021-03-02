using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;


[System.Serializable]
class ModelLink
{
    public int instanceID;
    public int assetID;
}

[System.Serializable]
class ModelCollection
{
    public List<ModelLink> modelLinks;
}

public class ExportModel
{
    public static void Export(Scene aScene, List<int> validInstanceIds)
    {
        ModelCollection fbxLinks = new ModelCollection();
        fbxLinks.modelLinks = new List<ModelLink>();

        Renderer[] allrenderers = GameObject.FindObjectsOfType<Renderer>();

        foreach(Renderer renderer in allrenderers)
        {
            if (renderer.GetComponent<PolybrushFBX>() != null)
                continue;

            if(Json.TryIsValidExport(renderer, out GameObject prefabParent))
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

                                if(!fbxLinks.modelLinks.Exists(e => e.instanceID == link.instanceID))
                                {
                                    fbxLinks.modelLinks.Add(link);
                                }
                            }
                        }
                    }
                    else
                    {
                        ModelLink link = new ModelLink();
                        link.assetID = modelAsset.transform.GetInstanceID();
                        link.instanceID = prefabParent.transform.GetInstanceID();
                        fbxLinks.modelLinks.Add(link);
                    }
                }
            }           
        }
        Json.ExportToJson(fbxLinks, aScene.name);
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
