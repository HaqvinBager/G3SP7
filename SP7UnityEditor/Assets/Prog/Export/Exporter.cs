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

public class Exporter 
{


    [MenuItem("Export/Export Scene")]
    static void ExportScene()
    {
        Scene currentScene = SceneManager.GetActiveScene();
        GameObject[] rootGameObjects = currentScene.GetRootGameObjects();

        for(int i = 0; i < rootGameObjects.Length; ++i)
        {
            SGameObject gameObject;
            gameObject.transform.position = rootGameObjects[i].transform.position;
            gameObject.transform.rotation = rootGameObjects[i].transform.rotation.eulerAngles;
            gameObject.transform.scale = rootGameObjects[i].transform.localScale;

            gameObject.model.fbxPath = AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromSource(rootGameObjects[i].GetComponent<MeshFilter>().sharedMesh)); 

            string jsonTransform = JsonUtility.ToJson(gameObject);
            string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\TestJson.json";
            System.IO.File.WriteAllText(savePath, jsonTransform);
        }

    }
    


}
