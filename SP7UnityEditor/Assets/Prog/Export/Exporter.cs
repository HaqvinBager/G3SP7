using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;
//Steg 1: Definera vilken data vi vill ha (EX: POs/Rotation/Model/instans ID)
//Steg 2: Hur får vi tag i datan från Unity
//Steg 3: Hur exporterar vi datan i JSon

[System.Serializable]
public struct STransform
{
    public float instanceID;
    public Vector3 position;
    public Vector3 rotation;
    public Vector3 scale;
}
[System.Serializable]
public struct SModel
{
    public string fbxPath;
}

[System.Serializable]
public struct SGameObject
{
    public STransform transform;
    public SModel model;
}

[System.Serializable]
public struct SScene
{
    public SGameObject[] gameobjects;
}


public class Exporter 
{


    [MenuItem("Export/Export Scene")]
    static void ExportScene()
    {
        Scene currentScene = SceneManager.GetActiveScene();
        GameObject[] rootGameObjects = currentScene.GetRootGameObjects();
        SScene SceneObject = new SScene();
        SceneObject.gameobjects = new SGameObject[rootGameObjects.Length];

        for(int i = 0; i < rootGameObjects.Length; ++i)
        {
            SGameObject gameObject;
            gameObject.transform.instanceID = rootGameObjects[i].GetInstanceID();
            gameObject.transform.position = rootGameObjects[i].transform.position;
            gameObject.transform.rotation = rootGameObjects[i].transform.ConvertToIronWroughtRotation();
                //rootGameObjects[i].transform.rotation.eulerAngles;
            gameObject.transform.scale = rootGameObjects[i].transform.localScale;

            gameObject.model.fbxPath = AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(rootGameObjects[i].GetComponent<Renderer>()));

            SceneObject.gameobjects[i] = gameObject;
        }

        string jsonGameObject = JsonUtility.ToJson(SceneObject);
        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\TestJson.json";
        System.IO.File.WriteAllText(savePath, jsonGameObject);
    }
}
