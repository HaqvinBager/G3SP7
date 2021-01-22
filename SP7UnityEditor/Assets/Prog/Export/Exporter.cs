using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;


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
public struct SInstancedGameObject
{
    public SModel model;
    public int count;
    public STransform[] transforms;
}

[System.Serializable]
public struct SScene
{
    public SInstancedGameObject[] instancedGameobjects;
}

public class Exporter 
{
    [MenuItem("Export/Export Scene")]
    static void ExportScene()
    {
        Scene currentScene = SceneManager.GetActiveScene();
        GameObject[] rootGameObjects = currentScene.GetRootGameObjects();
        SScene sceneObject = new SScene();
        Dictionary<string, List<STransform>> fbxPathGameObjectMap = new Dictionary<string, List<STransform>>();
        List<string> fbxpaths = new List<string>();
        for (int i = 0; i < rootGameObjects.Length; ++i)
        {
            string fbxPath = AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(rootGameObjects[i].GetComponent<Renderer>()));

            if (!fbxPathGameObjectMap.ContainsKey(fbxPath))
            {
                fbxPathGameObjectMap.Add(fbxPath, new List<STransform>());
            }
            STransform transform = new STransform();
            transform.instanceID = rootGameObjects[i].gameObject.GetInstanceID();
            transform.position = rootGameObjects[i].transform.position;
            transform.rotation = rootGameObjects[i].transform.ConvertToIronWroughtRotation();
            transform.scale = rootGameObjects[i].transform.localScale;
            fbxPathGameObjectMap[fbxPath].Add(transform);

            if (!fbxpaths.Contains(fbxPath))
            {
                fbxpaths.Add(fbxPath);
            }
           // Debug.Log(fbxPath + " Count: " + fbxPathMap[fbxPath]);
        }
        List<SInstancedGameObject> instancedGameObjects = new List<SInstancedGameObject>();
        for (int i = 0; i < fbxpaths.Count; ++i)
        {
            SInstancedGameObject instancedGameObject = new SInstancedGameObject();
            instancedGameObject.count = fbxPathGameObjectMap[fbxpaths[i]].Count;
            instancedGameObject.transforms = fbxPathGameObjectMap[fbxpaths[i]].ToArray();
            instancedGameObject.model.fbxPath = fbxpaths[i];
            instancedGameObjects.Add(instancedGameObject);
        }

        Debug.Log(currentScene.name);
        sceneObject.instancedGameobjects = instancedGameObjects.ToArray();
        string jsonGameObject = JsonUtility.ToJson(sceneObject);
        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\" + currentScene.name + ".json";
        System.IO.File.WriteAllText(savePath, jsonGameObject);
        AssetDatabase.Refresh();

    }
}

//NOTES
//DONE: Definera vilken data vi vill ha (EX: POs/Rotation/Model/instans ID)
//DONE: Hur får vi tag i datan från Unity
//DONE: Hur exporterar vi datan i JSon

// Exportera instansierade objekt
// DONE: FBX Path
// DONE: Count, antalet objekt av just denna model
// DONE: Lista med varje gameobjekt som tillhör en nyckel som är en FBX path

// DONE: Lista på varje objekts separata transforms
// DONE: Spara FBX paths i Json
// DONE: spara alla objekts transforms i Json
// DONE: sortera så att alla objekts transforms som har samma FBX hamnar under rätt plats i Json
// Exempel: 
//          En Fbx path för tex Chest \Assets\3D\Exempel_Modeller\Chest
//          Chest 1 + transform 
//           POS: X: 0.0 Y: 0.0 Z: 0.0  
//           ROT: X: 0.0 Y: 0.0 Z: 0.0 
//           Scale: 1
//          Chest 2 + transform data 
//          osv...
// Försa gången vi läser in ett objekt sparar vi Fbx path (Chest for example), sedan läser vi in en kub och sparar den FBX path.
// sedan möter vi en till chest modell och då sparar vi bara positionen och inte fbx path och count++

// LoadScene funktion / Class / Refaktorisera InGameState så vi kan få det mera strukturerat och tänka på att vi ska kunna bygga vidare på systemet. 
// (Export Levels) ta hela export scene konceptet och gör den "loopbar" så vi kan ta en samling av scener och exporta dem med ett klick. 
// AKA ExportLevelCollection
     