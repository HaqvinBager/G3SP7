using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;
using System.IO;
using System.Text;

[System.Serializable]
struct SceneIDCollection
{
    public List<int> Ids; 
}

public class ExportInstanceID 
{
    public static void Export(Scene aScene)
    {
        GameObject[] gameObjects = GameObject.FindObjectsOfType<GameObject>();

        SceneIDCollection sceneIDCollection = new SceneIDCollection();
        sceneIDCollection.Ids = new List<int>();

        foreach(GameObject gameObject in gameObjects)
        {
            sceneIDCollection.Ids.Add(gameObject.transform.GetInstanceID());
        }

        ExportToJson(sceneIDCollection, aScene.name, "InstanceIDs");
    }


    public static void ExportToJson<T>(T someDataToExport, string aSceneName, string aJsonTypeName)
    {

        StringBuilder jsonString = new StringBuilder();
        jsonString.Append(JsonUtility.ToJson(someDataToExport));

        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\";
        if (!System.IO.Directory.Exists(savePath))
            System.IO.Directory.CreateDirectory(savePath);

        savePath += aSceneName  + "_" + aJsonTypeName + ".json";
        System.IO.File.WriteAllText(savePath, jsonString.ToString());
    } 

}
