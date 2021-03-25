using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;

[System.Serializable]
public struct ModelAsset
{
    public int id;
    public string path;
}

[System.Serializable]
public struct Assets
{
    public List<ModelAsset> models;
}

[System.Serializable]
public struct PlayerID
{
    public int instanceID;
}

[System.Serializable]
public struct Player
{
    public PlayerID player;
}

public class Exporter
{

    [MenuItem("GameObject/BluePrint/Add Patrol Point", validate = true)]
    static bool ValidateTest()
    {
        if(Selection.gameObjects.Length == 1)
        {
            return Selection.gameObjects[0].name.Contains("BP_");
        }
        return false;
    }

    //[MenuItem("3D Create/PatrolPosition")]
    [MenuItem("GameObject/BluePrint/Add Patrol Point", false, 0)]
    static void Test()
    {
        PrefabUtility.InstantiatePrefab(AssetDatabase.LoadAssetAtPath<Object>("Assets/Prefabs/PatrolPoint.prefab"), Selection.activeTransform);
        //GameObject parent = Selection.gameObjects[0];
        //GameObject obj = new GameObject("Patrol");
        //obj.transform.parent = parent.transform;
    }

    [MenuItem("Export/Export Scene")]
    public static void Export()
    {
        ExportResource.Export("Resources");

        string sceneName = SceneManager.GetActiveScene().name;
        string baseSceneName = sceneName.Substring(0, sceneName.LastIndexOf('-') + 2);
        Json.BeginExport(baseSceneName);

        //for(int i = 0; i < SceneManager.sceneCount; ++i)
        //{
        //    Json.BeginScene(SceneManager.GetSceneAt(i).name);
        //    ExportAScene(SceneManager.GetSceneAt(i).name);
        //    Json.EndScene();
        //}

     
        //return;

        List<GameObject> allScenesActiveObjects = GetAllOpenedSceneActiveObjects();
        for (int i = 0; i < SceneManager.sceneCount; ++i)
        {
            Json.BeginScene(SceneManager.GetSceneAt(i).name);
            DeactivateAndExportScene(i, allScenesActiveObjects);
            Json.EndScene();
        }

        Json.EndExport(baseSceneName, baseSceneName);   
        foreach (var gameObject in allScenesActiveObjects)
            gameObject.SetActive(true);
    }

    private static void DeactivateAndExportScene(int aSceneIndex, List<GameObject> allScenesActiveObjects)
    {
        GameObject[] gameobjects = SceneManager.GetSceneAt(aSceneIndex).GetRootGameObjects();
        List<GameObject> activeobjects = new List<GameObject>();
        foreach (var gameobject in gameobjects)
        {
            if (allScenesActiveObjects.Contains(gameobject))
            {
                activeobjects.Add(gameobject);
                gameobject.SetActive(true);
            }
        }
    
        ExportAScene(SceneManager.GetSceneAt(aSceneIndex).name);
      //  Json.EndExport(SceneManager.GetSceneAt(aSceneIndex).name, "LevelData_" + SceneManager.GetSceneAt(aSceneIndex).name);

        foreach (var gameobject in activeobjects)
        {
            gameobject.SetActive(false);
        }

        AssetDatabase.Refresh();
    }

    private static void ExportAScene(string aSceneName)
    {

       //if (!aSceneName.Contains("Script") || !aSceneName.Contains("Lights"))
       //     return;

       // ModelCollection modelCollection = new ModelCollection();
       // modelCollection.modelLinks = new List<ModelLink>();
       // modelCollection.modelLinks.Add(new ModelLink { assetID = 100, instanceID = 1 });
       // //modelCollection.modelLinks.Add(new ModelLink { assetID = 101, instanceID = 2 });
       //// modelCollection.modelLinks.Add(new ModelLink { assetID = 102, instanceID = 3 });
       // Json.AddToExport(modelCollection);

       // Player player = new Player();
       // player.instanceID = 51252;
       // Json.AddToExport(player, true);

        //Json.AddToExport(test, );

        //Next Step is to make sure that we only write the correct data to the correct Scene_Json.json! 
        //Or maybe just leave it be tbh! This might just be a good enough of a solution!
        //Especially if we put these multiple .json files in a folder!
        InstanceIDCollection instanceIDs = ExportInstanceID.Export(aSceneName);
        Json.AddToExport(instanceIDs);
        Json.AddToExport(ExportTransform.Export(aSceneName, instanceIDs.Ids));
        Json.AddToExport(ExportModel.Export(aSceneName, instanceIDs.Ids));
        Json.AddToExport(ExportVertexPaint.Export(aSceneName, instanceIDs.Ids));
        Json.AddToExport(ExportPointlights.ExportPointlight(aSceneName));
        Json.AddToExport(ExportDecals.Export(aSceneName));
        Json.AddToExport(ExportPlayer(aSceneName));
        Json.AddToExport(ExportBluePrint.Export(aSceneName));
        Json.AddToExport(ExportInstancedModel.Export(aSceneName), true);

        //Json.EndExport(aSceneName, "");
    }

    private static Player ExportPlayer(string aSceneName)
    {
        Player data = new Player();
        PlayerSpawnPosition player = GameObject.FindObjectOfType<PlayerSpawnPosition>();
        if(player != null)
        {
            data.player.instanceID = player.transform.GetInstanceID();
            //Json.ExportToJson(data, aScene.name);
        }
        return data;
    }



    private static List<GameObject> GetAllOpenedSceneActiveObjects()
    {
        List<GameObject> allScenesActiveObjects = new List<GameObject>();
        for (int i = 0; i < SceneManager.sceneCount; ++i)
        {
            GameObject[] gameobjects = SceneManager.GetSceneAt(i).GetRootGameObjects();

            foreach (var gameobject in gameobjects)
            {
                if (gameobject.activeInHierarchy)
                {
                    allScenesActiveObjects.Add(gameobject);
                    gameobject.SetActive(false);
                }
            }
        }

        return allScenesActiveObjects;
    }
}