using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.SceneManagement;
using System.IO;

[CustomEditor(typeof(LevelCollection))]
public class LevelCollectionEditor : Editor
{
   

    //[MenuItem("Export/Connect Scenes")]
    //public static void ConnectScenes()
    //{
    //    List<string> sceneFolders = new List<string>(Directory.EnumerateDirectories(Application.dataPath + "/Scenes/"));
    //    foreach (var sceneFolder in sceneFolders)
    //    {
    //        string sceneName = sceneFolder.Substring(sceneFolder.LastIndexOf('/'), sceneFolder.Length - sceneFolder.LastIndexOf('/'));
    //        sceneName = sceneName.Substring(1, sceneName.Length - 1);
            
    //        string collectionAssetPath = sceneFolder + "\\" + sceneName + "_Scenes.asset";
    //        collectionAssetPath = collectionAssetPath.Substring(collectionAssetPath.LastIndexOf("Assets/"),  collectionAssetPath.Length - collectionAssetPath.LastIndexOf("Assets/"));

    //        LevelCollection collection = AssetDatabase.LoadAssetAtPath<LevelCollection>(collectionAssetPath);
    //        if(collection == null)
    //        {
    //            collection = ScriptableObject.CreateInstance<LevelCollection>();
    //            AssetDatabase.CreateAsset(collection, collectionAssetPath);
    //            AssetDatabase.SaveAssets();
    //            collection.Ping();
    //        }


    //        List<string> missingScenes = new List<string>();
    //        List<string> filePathsInFolder = new List<string>(Directory.EnumerateFiles(sceneFolder, "*.unity"));
    //        foreach (var assetPath in filePathsInFolder){
    //            string baseName = assetPath.Substring(assetPath.LastIndexOf('\\'), assetPath.Length - assetPath.LastIndexOf('\\'));
    //            baseName = baseName.Substring(1, baseName.Length - 1);             
    //            string unityAssetPath = assetPath.Substring(assetPath.LastIndexOf('\\'), assetPath.Length - assetPath.LastIndexOf('\\'));
    //            unityAssetPath = unityAssetPath.Substring(1, baseName.Length - 1);

    //            if(!collection.myScenes.Exists(e => e.ScenePath.Contains(unityAssetPath)))
    //            {
    //                missingScenes.Add(assetPath);
    //                Debug.Log("Found: " + assetPath);
    //            }
    //        }
    //    }
    //    AssetDatabase.Refresh();
    //    //string[] directories = Directory.GetDirectories(Application.persistentDataPath + "/Assets/);
    //    //List<string> sceneFolders = new List<string>();
    //    //foreach (string file in AssetDatabase.GetAllAssetPaths())
    //    //{
    //    //    if (file.Contains("Assets/Scenes/"))
    //    //    {
    //    //    }
    //    //}
    //}

    Object obj = null;

    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        serializedObject.Update();

        obj = EditorGUILayout.ObjectField("Scene Asset", obj, typeof(SceneAsset), false);    
        if (GUILayout.Button("Add Scene"))
        {
            SceneAsset sceneAsset = obj as SceneAsset;
            RegisterScene(obj);
        }

        if (GUILayout.Button("Open Scenes"))
        {
            LevelCollection levelCollection = (LevelCollection)target;
            EditorSceneManager.OpenScene(levelCollection.myScenes[0].ScenePath, OpenSceneMode.Single);
            for (int i = 1; i < levelCollection.myScenes.Count; ++i)
                EditorSceneManager.OpenScene(levelCollection.myScenes[i].ScenePath, OpenSceneMode.Additive);            
        }

        if(GUILayout.Button("Connect Scenes"))
        {
            string sceneFolder = AssetDatabase.GetAssetPath(target);
            sceneFolder = sceneFolder.Substring(0, sceneFolder.LastIndexOf('/'));

            List<string> missingScenes = new List<string>();
            List<string> filePathsInFolder = new List<string>(Directory.EnumerateFiles(sceneFolder, "*.unity"));

            LevelCollection collection = (LevelCollection)target;

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
                    //missingScenes.Add(assetPath);
                    Debug.Log("Want to Register: " + assetPath);
                }
            }        
        }

        if (GUILayout.Button("Export"))
        {


        }

        serializedObject.ApplyModifiedProperties();
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
