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
public struct Player
{
    public int instanceID;
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
        GameObject parent = Selection.gameObjects[0];

        GameObject obj = new GameObject("Patrol");
        obj.transform.parent = parent.transform;
    }

    [MenuItem("Export/Export Scene")]
    static void ExportScene()
    {
        List<GameObject> allScenesActiveObjects = GetAllOpenedSceneActiveObjects();
        for (int i = 0; i < SceneManager.sceneCount; ++i)
            DeactivateAndExportScene(i, allScenesActiveObjects);

        foreach (var gameObject in allScenesActiveObjects)
            gameObject.SetActive(true);
    }

    private static void ExportAScene(Scene aScene)
    {
        ExportResource.Export(aScene);
        List<int> validExportIds = ExportInstanceID.Export(aScene);
        ExportTransform.Export(aScene, validExportIds);
        ExportModel.Export(aScene, validExportIds);
        ExportInstancedModel.Export(aScene);
        ExportVertexPaint.ExportVertexPainting(aScene, validExportIds);
        ExportBluePrint.Export(aScene);
        ExportPointlights.ExportPointlight(aScene);
        ExportDecals.Export(aScene);
        ExportPlayer(aScene);
        AssetDatabase.Refresh();
    }

    private static void ExportPlayer(Scene aScene)
    {
        PlayerSpawnPosition player = GameObject.FindObjectOfType<PlayerSpawnPosition>();
        if(player != null)
        {
            Player data = new Player();
            data.instanceID = player.transform.GetInstanceID();
            Json.ExportToJson(data, aScene.name);
        }
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

        ExportAScene(SceneManager.GetSceneAt(aSceneIndex));

        foreach (var gameobject in activeobjects)
        {
            gameobject.SetActive(false);
        }
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


    [MenuItem("Update Events")]
    public static void UpdateEvents()
    {
        




    }
}


//List<GameObject> alreadyExportedRenderers =
//ExportBlueprints.ExportBluePrint(aScene);
//ExportPointlights.ExportPointlight(aScene);
//ExportFbx.Export(aScene);

//Renderer[] allrenderers = GameObject.FindObjectsOfType<Renderer>();
//Dictionary<string, List<STransform>> fbxPathGameObjectMap = new Dictionary<string, List<STransform>>();
//List<string> fbxpaths = new List<string>();

//for (int i = 0; i < allrenderers.Length; ++i)
//{
//    if (alreadyExportedRenderers.Contains(allrenderers[i].gameObject))
//        continue;

//    string fbxPath = AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(allrenderers[i].GetComponent<MeshFilter>().sharedMesh));
//    if (!fbxPathGameObjectMap.ContainsKey(fbxPath))
//        fbxPathGameObjectMap.Add(fbxPath, new List<STransform>());

//    STransform transform = new STransform();

//    GameObject prefabParent = PrefabUtility.GetOutermostPrefabInstanceRoot(allrenderers[i]);
//    prefabParent.Ping();


//    transform.instanceID = prefabParent.transform.GetInstanceID();
//    //allrenderers[i].transform.GetInstanceID();
//    transform.position = allrenderers[i].transform.position;
//    transform.rotation = allrenderers[i].transform.ConvertToIronWroughtRotation();
//    transform.scale = allrenderers[i].transform.localScale;
//    fbxPathGameObjectMap[fbxPath].Add(transform);

//    if (!fbxpaths.Contains(fbxPath))
//    {
//        fbxpaths.Add(fbxPath);
//    }
//}

//List<SInstancedGameObject> instancedGameObjects = new List<SInstancedGameObject>();
//for (int i = 0; i < fbxpaths.Count; ++i)
//{
//    SInstancedGameObject instancedGameObject = new SInstancedGameObject();
//    instancedGameObject.count = fbxPathGameObjectMap[fbxpaths[i]].Count;
//    instancedGameObject.transforms = fbxPathGameObjectMap[fbxpaths[i]].ToArray();
//    instancedGameObject.model.fbxPath = fbxpaths[i];
//    instancedGameObjects.Add(instancedGameObject);
//}

//List<SGameObject> gameObjects = new List<SGameObject>();
//foreach (var vertexPaintedObject in alreadyExportedRenderers)
//{
//    //Få tag i original-FBXen som användes till denna Vertex-paintade gameobject
//    //string fbxPath = AssetDatabase.GetAssetPath(
//    //    PrefabUtility.GetCorrespondingObjectFromOriginalSource(
//    //         renderer.GetComponent<PolybrushMesh>().m_OriginalMeshObject));

//    if (vertexPaintedObject.TryGetComponent(out PolybrushFBX polyBrushFBX))
//    {
//        SGameObject gameObject = new SGameObject();
//        string fbxPath = AssetDatabase.GUIDToAssetPath(polyBrushFBX.originalFBXGUID);
//        gameObject.model.fbxPath = fbxPath;

//        GameObject prefabParent = PrefabUtility.GetOutermostPrefabInstanceRoot(vertexPaintedObject);

//        gameObject.transform.instanceID = prefabParent.transform.GetInstanceID();
//        gameObject.transform.position = vertexPaintedObject.transform.ConvertToIronWroughtPosition();
//        gameObject.transform.rotation = vertexPaintedObject.transform.ConvertToIronWroughtRotation();
//        gameObject.transform.scale = vertexPaintedObject.transform.ConvertToIronWroughtScale();
//        gameObjects.Add(gameObject);
//    }
//}

//SScene sceneObject = new SScene();
//sceneObject.instancedGameobjects = instancedGameObjects.ToArray();
//sceneObject.modelGameObjects = gameObjects.ToArray();
//string jsonGameObject = JsonUtility.ToJson(sceneObject);
//string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\";
//if (!System.IO.Directory.Exists(savePath))
//    System.IO.Directory.CreateDirectory(savePath);
//savePath += aScene.name + ".json";
//System.IO.File.WriteAllText(savePath, jsonGameObject);
//AssetDatabase.Refresh();