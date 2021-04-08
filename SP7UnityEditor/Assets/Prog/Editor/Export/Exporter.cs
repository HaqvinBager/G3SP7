using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;
using System.IO;


[System.Serializable]
public struct PlayerID
{
    public int instanceID;
    public List<int> childrenIDs;
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

        //string sceneName = SceneManager.GetActiveScene().name;
       // string baseSceneName = sceneName.Substring(0, sceneName.LastIndexOf('_') + 2);
        DirectoryInfo parentDirectory = Directory.GetParent(SceneManager.GetActiveScene().path);
        Json.BeginExport(parentDirectory.Name);

        List<GameObject> allScenesActiveObjects = GetAllOpenedSceneActiveObjects();
        for (int i = 0; i < SceneManager.sceneCount; ++i)
        {
            Json.BeginScene(SceneManager.GetSceneAt(i).name);
            DeactivateAndExportScene(i, allScenesActiveObjects);
            Json.EndScene();
        }

        Json.EndExport(parentDirectory.Name, parentDirectory.Name);

        foreach (var gameObject in allScenesActiveObjects)
            gameObject.SetActive(true);

        ExportResource.Export("Resources");
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

        foreach (var gameobject in activeobjects)
        {
            gameobject.SetActive(false);
        }

        AssetDatabase.Refresh();
    }

    private static void ExportAScene(string aSceneName)
    {
        //Next Step is to make sure that we only write the correct data to the correct Scene_Json.json!
        //Or maybe just leave it be tbh! This might just be a good enough of a solution!
        //Especially if we put these multiple .json files in a folder!
        InstanceIDCollection instanceIDs = ExportInstanceID.Export(aSceneName);
        Json.AddToExport(instanceIDs);
        Json.AddToExport(ExportTransform.Export(aSceneName, instanceIDs.Ids));
        Json.AddToExport(ExportVertexPaint.Export(aSceneName, instanceIDs.Ids));
        Json.AddToExport(ExportModel.Export(aSceneName, instanceIDs.Ids));
        Json.AddToExport(ExportDirectionalLight.Export(aSceneName));
        Json.AddToExport(ExportPointlights.ExportPointlight(aSceneName));
        Json.AddToExport(ExportDecals.Export(aSceneName));
        Json.AddToExport(ExportPlayer(aSceneName));
        Json.AddToExport(ExportBluePrint.Export(aSceneName));
        Json.AddToExport(ExportCollider.Export(aSceneName, instanceIDs.Ids));
        Json.AddToExport(EnemyExporter.Export(aSceneName));
        Json.AddToExport(ExportInstancedModel.Export(aSceneName), true);
    }

    private static Player ExportPlayer(string aSceneName)
    {
        Player data = new Player();
        PlayerSpawnPosition player = GameObject.FindObjectOfType<PlayerSpawnPosition>();
        if(player != null)
        {
            data.player.instanceID = player.transform.GetInstanceID();
            data.player.childrenIDs = new List<int>();
            foreach (Transform child in player.transform)
                data.player.childrenIDs.Add(child.GetInstanceID());
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
