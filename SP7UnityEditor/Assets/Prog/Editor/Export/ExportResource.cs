using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

public class ExportResource 
{
    public static void Export(Scene aScene)
    {
        ExportModelAssets(aScene);

    }


    private static void ExportModelAssets(Scene aScene)
    {
        string[] allAssetPaths = AssetDatabase.GetAllAssetPaths();
        Assets assets = new Assets();
        assets.models = new List<ModelAsset>();
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
                        
                    }

                }
            }
        }
        Json.ExportToJson(assets, "Resource");
    }

    public static void ExportBluePrintPrefabs(Scene aScene)
    {
        BluePrintPrefabs collection = new BluePrintPrefabs();
        collection.blueprintPrefabs = new List<BluePrintAsset>();
        var prefabs = ExportBluePrint.LoadBluePrintPrefabGameObjects();
        foreach (var prefab in prefabs)
        {
            BluePrintAsset link = new BluePrintAsset();
            link.id = prefab.transform.GetInstanceID();
            link.type = prefab.name;
            link.childCount = prefab.transform.childCount;

            collection.blueprintPrefabs.Add(link);
        }

        Json.ExportToJson(collection, "Resource");
    }


}
