using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.SceneManagement;
using System.IO;
using UnityEngine.SceneManagement;

[CustomEditor(typeof(LevelCollection))]
public class LevelCollectionEditor : Editor
{

    [MenuItem("Export/Create Missing LevelCollections", true)]
    static bool Validate()
    {
        List<string> sceneFolders = new List<string>(Directory.EnumerateDirectories(Application.dataPath + "/Scenes/"));
        foreach (var sceneFolder in sceneFolders)
        {
            string sceneName = sceneFolder.Substring(sceneFolder.LastIndexOf('/'), sceneFolder.Length - sceneFolder.LastIndexOf('/'));
            sceneName = sceneName.Substring(1, sceneName.Length - 1);

            string collectionAssetPath = sceneFolder + "\\" + sceneName + "_Scenes.asset";
            collectionAssetPath = collectionAssetPath.Substring(collectionAssetPath.LastIndexOf("Assets/"), collectionAssetPath.Length - collectionAssetPath.LastIndexOf("Assets/"));

            LevelCollection collection = AssetDatabase.LoadAssetAtPath<LevelCollection>(collectionAssetPath);
            if (collection == null)
            {
                return true;
            }
        }
        return false;
    }

    [MenuItem("Export/Create Missing LevelCollections", false)]
    static void ConnectScenes()
    {
        List<string> sceneFolders = new List<string>(Directory.EnumerateDirectories(Application.dataPath + "/Scenes/"));
        foreach (var sceneFolder in sceneFolders)
        {
            string sceneName = sceneFolder.Substring(sceneFolder.LastIndexOf('/'), sceneFolder.Length - sceneFolder.LastIndexOf('/'));
            sceneName = sceneName.Substring(1, sceneName.Length - 1);

            string collectionAssetPath = sceneFolder + "\\" + sceneName + "_Scenes.asset";
            collectionAssetPath = collectionAssetPath.Substring(collectionAssetPath.LastIndexOf("Assets/"), collectionAssetPath.Length - collectionAssetPath.LastIndexOf("Assets/"));

            LevelCollection collection = AssetDatabase.LoadAssetAtPath<LevelCollection>(collectionAssetPath);
            if (collection == null)
            {
                collection = ScriptableObject.CreateInstance<LevelCollection>();
                AssetDatabase.CreateAsset(collection, collectionAssetPath);
                AssetDatabase.SaveAssets();
                collection.Ping();
            }
        }
        AssetDatabase.Refresh();
    }


    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        if (this != null)
            base.OnInspectorGUI();
    
        LevelCollection collection = (LevelCollection)target;
        Color originalColor = GUI.backgroundColor;

        EditorGUILayout.BeginHorizontal();
            OpenScenes(originalColor);
            ConnectScenes(collection, originalColor);
        EditorGUILayout.EndHorizontal();
            ExportScenes(collection);

        GUI.backgroundColor = originalColor;
        serializedObject.ApplyModifiedProperties();
    }

    private void ExportScenes(LevelCollection collection)
    {
        GUI.backgroundColor = collection.myScenes.Count > 0 ? Color.green : Color.red;
        if (GUILayout.Button("Export"))
        {
            OpenScenes();
            ExporterJson.ExportJson();
        }
    }

    private void OpenScenes(Color originalColor)
    {
        GUI.backgroundColor = Color.yellow;
        if (GUILayout.Button("Open Level"))
        {
            OpenScenes();
        }
        GUI.backgroundColor = originalColor;
    }

    private void ConnectScenes(LevelCollection collection, Color originalColor)
    {
        GUI.backgroundColor = GUI.backgroundColor = collection.myScenes.Count <= 0 ? Color.green : Color.Lerp(Color.cyan, Color.white, 0.25f);
        if (GUILayout.Button("Connect Scenes"))
        {
            string sceneFolder = AssetDatabase.GetAssetPath(target);
            sceneFolder = sceneFolder.Substring(0, sceneFolder.LastIndexOf('/'));
            List<string> filePathsInFolder = new List<string>(Directory.EnumerateFiles(sceneFolder, "*.unity"));

            serializedObject.Update();
            var myScenes = serializedObject.FindProperty("myScenes");
            myScenes.ClearArray();
            serializedObject.ApplyModifiedProperties();

            foreach (var assetPath in filePathsInFolder)
            {
                string baseName = assetPath.Substring(assetPath.LastIndexOf('\\'), assetPath.Length - assetPath.LastIndexOf('\\'));
                baseName = baseName.Substring(1, baseName.Length - 1);
                string unityAssetPath = assetPath.Substring(assetPath.LastIndexOf('\\'), assetPath.Length - assetPath.LastIndexOf('\\'));
                unityAssetPath = unityAssetPath.Substring(1, baseName.Length - 1);

                if (!collection.myScenes.Exists(e => e.ScenePath.Contains(unityAssetPath)))
                {
                    SceneAsset sceneAsset = AssetDatabase.LoadAssetAtPath<SceneAsset>(assetPath);
                    RegisterScene(sceneAsset);
                }
            }
        }
        GUI.backgroundColor = originalColor;
    }

    private void OpenScenes()
    {
        LevelCollection levelCollection = (LevelCollection)target;
        EditorSceneManager.OpenScene(levelCollection.myScenes[0].ScenePath, OpenSceneMode.Single);
        for (int i = 1; i < levelCollection.myScenes.Count; ++i)
            EditorSceneManager.OpenScene(levelCollection.myScenes[i].ScenePath, OpenSceneMode.Additive);
    }

    private void RegisterScene(Object obj)
    {
        var myScenes = serializedObject.FindProperty("myScenes");
        int size = myScenes.arraySize;
        myScenes.InsertArrayElementAtIndex(size);
        var newElement = myScenes.GetArrayElementAtIndex(size);
        newElement.FindPropertyRelative("sceneAsset").objectReferenceValue = obj;
        obj = null;
    }
}
