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
    public int vertexColorID;
}

[System.Serializable]
public class ModelCollection
{
    public List<ModelLink> models;
}

public class ExportModel
{
    public static ModelCollection Export(string aSceneName, List<int> validInstanceIds)
    {
        ModelCollection modelCollection = new ModelCollection();
        modelCollection.models = new List<ModelLink>();

        Renderer[] allrenderers = GameObject.FindObjectsOfType<Renderer>();

        foreach(Renderer renderer in allrenderers)
        {
            if (renderer.sharedMaterial.shader.name.Contains("Decal"))
                continue;

            bool isLiterallyVertexPainted = false;
            if(renderer.TryGetComponent(out MeshFilter filter))
            {
                isLiterallyVertexPainted = filter.sharedMesh.name.Contains("PolybrushMesh");
            }

            if (isLiterallyVertexPainted)
            {
                if (renderer.TryGetComponent(out PolybrushFBX polyBrushFbx))
                {
                    string assetPath = AssetDatabase.GUIDToAssetPath(polyBrushFbx.originalFBXGUID);
                    GameObject modelAsset = AssetDatabase.LoadAssetAtPath<GameObject>(assetPath);

                    ModelLink link = new ModelLink();
                    link.assetID = modelAsset.transform.GetInstanceID();
                    link.instanceID = renderer.transform.parent.GetInstanceID();

                    string materialName = renderer.sharedMaterial.name;
                    string meshName = renderer.GetComponent<MeshFilter>().sharedMesh.name;
                    meshName = meshName.Substring(meshName.LastIndexOf('h'), meshName.Length - meshName.LastIndexOf('h'));
                    meshName = meshName.Substring(1, meshName.Length - 1);
                    //link.vertexColorID = AssetDatabase.LoadAssetAtPath<Object>("Assets/Generated/VertexColors/VertexColors_" + meshName.ToString() + "_Bin.bin").GetInstanceID();
                    link.vertexColorID = AssetDatabase.LoadAssetAtPath<Object>("Assets/Generated/VertexColors/VertexColors_" + polyBrushFbx.GetInstanceID().ToString() + "_Bin.bin").GetInstanceID();


                    if (!modelCollection.models.Exists(e => e.instanceID == link.instanceID))
                        modelCollection.models.Add(link);

                    continue;
                }
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

                                if(!modelCollection.models.Exists(e => e.instanceID == link.instanceID))
                                {
                                    modelCollection.models.Add(link);
                                }
                            }
                        }
                    }
                    else
                    {
                        ModelLink link = new ModelLink();
                        link.assetID = modelAsset.transform.GetInstanceID();
                        link.instanceID = prefabParent.transform.GetInstanceID();
                        modelCollection.models.Add(link);
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
