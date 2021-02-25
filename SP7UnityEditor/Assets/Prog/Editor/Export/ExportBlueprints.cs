using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Text;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct BluePrintCollection
{
    public List<BluePrintLinker> links;
}

[System.Serializable]
public struct BluePrintLinker
{
    public string type;
    public List<int> instances;
}


public class ExportBlueprints
{
    private static string SavePath
    {
        get => System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\";
    }

    private static string PrefabPath
    {
        get => System.IO.Directory.GetCurrentDirectory() + "/Assets/Prefabs/";
    }

    //[MenuItem("Export/Blueprint")]
    public static void ExportBluePrint(Scene aScene)
    {
        List<GameObject> bluePrintPrefabs = LoadBluePrintPrefabGameObjects();
        GameObject[] allGameObjects = GameObject.FindObjectsOfType<GameObject>();

        BluePrintCollection bluePrintCollection = new BluePrintCollection();
        bluePrintCollection.links = new List<BluePrintLinker>();

        foreach (GameObject prefab in bluePrintPrefabs)
        {
            BluePrintLinker collection = new BluePrintLinker();
            collection.instances = new List<int>();
            collection.type = prefab.name;

            foreach (GameObject gameObject in allGameObjects)
            {
                if (gameObject.name.Contains(collection.type))
                {
                    GameObject source = PrefabUtility.GetCorrespondingObjectFromSource(gameObject);
                    if (source != null)
                    {
                        if (source.Equals(prefab))
                        {
                            collection.instances.Add(gameObject.transform.GetInstanceID());
                        }
                    }
                }
            }
            if (collection.instances.Count > 0)
            {
                bluePrintCollection.links.Add(collection);
            }
        }

        if (!System.IO.Directory.Exists(SavePath))
            System.IO.Directory.CreateDirectory(SavePath);


        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.Append(JsonUtility.ToJson(bluePrintCollection));

        string savePath = SavePath + "BluePrintLinks_" + aScene.name + ".json";
        System.IO.File.WriteAllText(savePath, stringBuilder.ToString());
        AssetDatabase.Refresh();
    }

    private static List<GameObject> LoadBluePrintPrefabGameObjects()
    {
        List<GameObject> bluePrintPrefabs = new List<GameObject>();
        List<string> bluePrintPrefabPaths = GetBlueprintPrefabPaths();

        foreach (string bluePrintPath in bluePrintPrefabPaths)
        {
            bluePrintPrefabs.Add(AssetDatabase.LoadAssetAtPath<GameObject>(bluePrintPath));
        }
        return bluePrintPrefabs;
    }

    private static List<string> GetBlueprintPrefabPaths()
    {
        List<string> bluePrintPrefabs = new List<string>();
        string[] allFilePaths = AssetDatabase.GetAllAssetPaths();
        //Directory.GetFiles(System.IO.Directory.GetCurrentDirectory() + "/Assets/Prefabs/", "BP_*", SearchOption.AllDirectories);

        foreach (string path in allFilePaths)
        {
            FileInfo info = new FileInfo(path);
            if (info.Extension != ".meta")
            {
                if (info.Extension == ".prefab")
                {
                    if (info.Name.Contains("BP_"))
                        bluePrintPrefabs.Add(path);
                }
            }
        }

        return bluePrintPrefabs;
    }
}
