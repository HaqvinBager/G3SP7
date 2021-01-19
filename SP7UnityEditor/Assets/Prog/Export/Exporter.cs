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
public struct SInstancedGameObject
{
    public SModel model;
    public int count;
    public STransform[] transforms;
}

[System.Serializable]
public struct SScene
{
    //public SGameObject[] gameobjects;
    public SInstancedGameObject[] instancedGameobjects;
    public bool ContainsFBX(Renderer aRenderer)
    {

        return true; 
    }
}

public class Exporter 
{
    [MenuItem("Export/Export Scene")]
    static void ExportScene()
    {
        Scene currentScene = SceneManager.GetActiveScene();
        GameObject[] rootGameObjects = currentScene.GetRootGameObjects();
        SScene sceneObject = new SScene();
        // SceneObject.gameobjects = new SGameObject[rootGameObjects.Length];
        Dictionary<string, int> fbxPathMap = new Dictionary<string, int>();
        Dictionary<string, List<GameObject>> fbxPathGameObjectMap = new Dictionary<string, List<GameObject>>();

        for (int i = 0; i < rootGameObjects.Length; ++i)
        {
            string fbxPath = AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(rootGameObjects[i].GetComponent<Renderer>()));

            if (!fbxPathGameObjectMap.ContainsKey(fbxPath))
            {
                fbxPathGameObjectMap.Add(fbxPath, new List<GameObject>());
            }

            fbxPathGameObjectMap[fbxPath].Add(rootGameObjects[i]);


            if (!fbxPathMap.ContainsKey(fbxPath))
            {
                fbxPathMap.Add(fbxPath, 0);
            }
            fbxPathMap[fbxPath]++;
            //Debug.Log(fbxPath + " Count: " + fbxPathMap[fbxPath]);
        }

        string jsonGameObject = JsonUtility.ToJson(sceneObject);
        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\TestJson.json";
        System.IO.File.WriteAllText(savePath, jsonGameObject);
    }
}

//SInstancedGameObject instancedGameObject;

//if (sceneObject.ContainsFBX(rootGameObjects[i].GetComponent<Renderer>()) == false)
//{
//    instancedGameObject.model.fbxPath = AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(rootGameObjects[i].GetComponent<Renderer>()));
//}

////instancedGameObject.transform.instanceID = rootGameObjects[i].GetInstanceID();
////instancedGameObject.transform.position = rootGameObjects[i].transform.position;
////instancedGameObject.transform.rotation = rootGameObjects[i].transform.ConvertToIronWroughtRotation();
////instancedGameObject.transform.scale = rootGameObjects[i].transform.localScale;


//// SceneObject.gameobjects[i] = gameObject;



//NOTES

// Exportera instansierade objekt
// step 1: FBX Path
// step 2: Count, antalet objekt av just denna model
// step 3: Lista med varje gameobjekt som tillhör en nyckel som är en FBX path
// step 4: Lista på varje objekts separata transforms


// LoadScene funktion / Class / Refaktorisera InGameState så vi kan få det mera strukturerat och tänka på att vi ska kunna bygga vidare på systemet. 
// (Export Levels) ta hela export scene konceptet och gör den "loopbar" så vi kan ta en samling av scener och exporta dem med ett klick. 
// AKA ExportLevelCollection

// Försa gången vi läser in ett objekt sparar vi Fbx path (Chest for example), sedan läser vi in en kub och sparar den FBX path.
// sedan möter vi en till chest modell och då sparar vi bara positionen och inte fbx path och count++