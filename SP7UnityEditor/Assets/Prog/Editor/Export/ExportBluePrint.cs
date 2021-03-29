using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Text;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct BluePrintInstance
{
    public int instanceID;
    public List<int> childrenInstanceIDs;
}

[System.Serializable]
public struct BluePrintLinker
{
    public int id;
    public string type;
    public List<BluePrintInstance> instances;
}

[System.Serializable]
public struct BluePrintCollection
{
    public List<BluePrintLinker> bluePrints;
}

//[System.Serializable]
//public struct BluePrintPrefabs
//{
//    public List<BluePrintAsset> blueprintPrefabs;
//}

//[System.Serializable]
//public struct BluePrintAsset
//{
//    public int id;
//    public string type;
//    public int childCount;
//}

public class ExportBluePrint
{
    public static BluePrintCollection Export(string aScene)
    {
        List<GameObject> bluePrintPrefabs = LoadBluePrintPrefabGameObjects();
        GameObject[] allGameObjects = GameObject.FindObjectsOfType<GameObject>();

        BluePrintCollection collection = new BluePrintCollection();
        collection.bluePrints = new List<BluePrintLinker>();

        foreach (GameObject prefab in bluePrintPrefabs)
        {
            BluePrintLinker link = new BluePrintLinker();
            link.instances = new List<BluePrintInstance>();
            link.type = prefab.name;
            link.id = prefab.transform.GetInstanceID();

            foreach (GameObject gameObject in allGameObjects)
            {
                GameObject source = PrefabUtility.GetCorrespondingObjectFromSource(gameObject);
                if(prefab == source)
                {
                    BluePrintInstance instance = new BluePrintInstance();
                    instance.instanceID = gameObject.transform.GetInstanceID();

                    instance.childrenInstanceIDs = new List<int>();
                    foreach (Transform childTransform in gameObject.transform)
                    {
                        instance.childrenInstanceIDs.Add(childTransform.GetInstanceID());
                    }
                    link.instances.Add(instance);
                }


                //if (gameObject.name.Contains(link.type))
                //{
                //    if (source != null)
                //    {
                //        if (source.Equals(prefab))
                //        {
                          
                //        }
                //    }
                //}
            }
            if (link.instances.Count > 0)
            {
                collection.bluePrints.Add(link);
            }
        }

        return collection;
        //Json.ExportToJson(collection, aScene.name);
    }

    public static List<GameObject> LoadBluePrintPrefabGameObjects()
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
