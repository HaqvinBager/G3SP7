using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;
using UnityEditor.SceneManagement;


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


        for (int i = 0; i < SceneManager.sceneCount; ++i)
        {
            GameObject[] gameobjects = SceneManager.GetSceneAt(i).GetRootGameObjects();
            List<GameObject> activeobjects = new List<GameObject>();
            foreach (var gameobject in gameobjects)
            {
                if (allScenesActiveObjects.Contains(gameobject))
                {
                    activeobjects.Add(gameobject);
                    gameobject.SetActive(true);
                }
            }


            ExportAScene(SceneManager.GetSceneAt(i));

            foreach (var gameobject in activeobjects)
            {
                gameobject.SetActive(false);
            }
        }

        foreach (var gameObject in allScenesActiveObjects)
        {
            gameObject.SetActive(true);
        }

    }

    private static void ExportAScene(Scene aScene)
    {

        //GameObject[] rootGameObjects = aScene.GetRootGameObjects();
        Renderer[] allrenderers = GameObject.FindObjectsOfType<Renderer>();


        Dictionary<string, List<STransform>> fbxPathGameObjectMap = new Dictionary<string, List<STransform>>();
        List<Renderer> renderers = new List<Renderer>();

        List<string> fbxpaths = new List<string>();


        //foreach (Renderer go in allrenderers)
        for (int i = 0; i < allrenderers.Length; ++i)
        //for (int i = 0; i < rootGameObjects.Length; ++i)

        {

            string fbxPath = AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(allrenderers[i].GetComponent<Renderer>()));

            if (!fbxPathGameObjectMap.ContainsKey(fbxPath))
            {
                fbxPathGameObjectMap.Add(fbxPath, new List<STransform>());
            }
            STransform transform = new STransform();
            transform.instanceID = allrenderers[i].gameObject.GetInstanceID();
            transform.position = allrenderers[i].transform.position;
            transform.rotation = allrenderers[i].transform.ConvertToIronWroughtRotation();
            transform.scale = allrenderers[i].transform.localScale;
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

        Debug.Log(aScene.name);
        SScene sceneObject = new SScene();
        sceneObject.instancedGameobjects = instancedGameObjects.ToArray();
        string jsonGameObject = JsonUtility.ToJson(sceneObject);
        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Level_Json\\" + aScene.name + ".json";
        System.IO.File.WriteAllText(savePath, jsonGameObject);
        AssetDatabase.Refresh();
    }
}