using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct ModelAsset
{
    public int id;
    public string path;
}

[System.Serializable]
public struct VertexColorAsset
{
    public int id;
    public string path;
}

[System.Serializable]
public struct Assets
{
    public List<ModelAsset> models;
    public List<VertexColorAsset> vertexColors;
}

public class ExportResource 
{
    public static void Export(string aSceneName)
    {
        ExportModelAssets(aSceneName);
    }


    public static Assets ExportModelAssets(string aSceneName)
    {
        string[] allAssetPaths = AssetDatabase.GetAllAssetPaths();
        Assets assets = new Assets();
        assets.models = new List<ModelAsset>();
        assets.vertexColors = new List<VertexColorAsset>();

        foreach (string assetPath in allAssetPaths)
        {
            if (assetPath.Contains("Graphics"))
            {
                Object asset = AssetDatabase.LoadAssetAtPath<Object>(assetPath);
                if (asset != null)
                {
                    if (PrefabUtility.GetPrefabAssetType(asset) == PrefabAssetType.Model)
                    {
                        GameObject gameObject = asset as GameObject;
                        ModelAsset modelAsset = new ModelAsset();
                        modelAsset.id = gameObject.transform.GetInstanceID();
                        modelAsset.path = assetPath;
                        assets.models.Add(modelAsset);
                    }
                    else
                    {
                        //Add Other types of Resources
                        //Textures
                        //Crazy references to values or something
                    }
                }
            }
            else if (assetPath.Contains("/Generated/"))
            {
                if (assetPath.Contains("/VertexColors/"))
                {
                    Object asset = AssetDatabase.LoadAssetAtPath<Object>(assetPath);
                    VertexColorAsset vertexColorAsset = new VertexColorAsset();
                    vertexColorAsset.id = asset.GetInstanceID();
                    vertexColorAsset.path = assetPath;
                    assets.vertexColors.Add(vertexColorAsset);
                }
            }
        }
        Json.ExportToJson(assets, "Resource");
        return assets;
    }

    //public static void ExportBluePrintPrefabs(Scene aScene)
    //{
    //    BluePrintPrefabs collection = new BluePrintPrefabs();
    //    collection.blueprintPrefabs = new List<BluePrintAsset>();
    //    var prefabs = ExportBluePrint.LoadBluePrintPrefabGameObjects();
    //    foreach (var prefab in prefabs)
    //    {
    //        BluePrintAsset link = new BluePrintAsset();
    //        link.id = prefab.transform.GetInstanceID();
    //        link.type = prefab.name;
    //        link.childCount = prefab.transform.childCount;

    //        collection.blueprintPrefabs.Add(link);
    //    }

    //    Json.ExportToJson(collection, "Resource");
    //}


}
