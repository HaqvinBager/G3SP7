using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine.Polybrush;





[System.Serializable]
public struct SModel
{
    public string fbxPath;
}

[System.Serializable]
public struct SGameObject
{
    public TransformLink transform;
    public SModel model;
}
[System.Serializable]
public struct SInstancedGameObject
{
    public SModel model;
    public int count;
    public TransformLink[] transforms;
}

[System.Serializable]
public struct SScene
{
    public SInstancedGameObject[] instancedGameobjects;
    public SGameObject[] modelGameObjects;
}

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
        ExportModelAssets(aScene);
        ExportInstanceID.Export(aScene);
        ExportTransform.Export(aScene);
        ExportModel.Export(aScene);
        ExportInstancedModel.Export(aScene);
        ExportVertexPaint.ExportVertexPainting(aScene);
        ExportBlueprints.ExportBluePrint(aScene);
        AssetDatabase.Refresh();      
    }


    public static void ExportModelAssets(Scene aScene)
    {
        string[] allAssetPaths = AssetDatabase.GetAllAssetPaths();
        Assets assets = new Assets();
        assets.models = new List<ModelAsset>();
        foreach (string assetPath in allAssetPaths)
        {
            if (assetPath.Contains("Graphics"))
            {
                GameObject asset = AssetDatabase.LoadAssetAtPath<GameObject>(assetPath);
                if(asset != null)
                {
                    if(PrefabUtility.GetPrefabAssetType(asset) == PrefabAssetType.Model)
                    {
                        ModelAsset modelAsset = new ModelAsset();
                        modelAsset.id = asset.transform.GetInstanceID();
                        modelAsset.path = assetPath;
                        assets.models.Add(modelAsset);
                    }
                }
            }
        }
        Json.ExportToJson(assets, "Resource");
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