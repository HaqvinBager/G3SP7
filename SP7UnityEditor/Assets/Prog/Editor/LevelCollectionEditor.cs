using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.SceneManagement;

[CustomEditor(typeof(LevelCollection))]
public class LevelCollectionEditor : Editor
{
    Object obj = null;

    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        serializedObject.Update();

        obj = EditorGUILayout.ObjectField("Scene Asset", obj, typeof(SceneAsset), false);
       
        SceneAsset sceneAsset = obj as SceneAsset;   
        if (GUILayout.Button("Add Scene"))
        {
            var myScenes = serializedObject.FindProperty("myScenes");
            int size = myScenes.arraySize;
            myScenes.InsertArrayElementAtIndex(size);
            var newElement = myScenes.GetArrayElementAtIndex(size);
            newElement.FindPropertyRelative("sceneAsset").objectReferenceValue = obj;
            obj = null;
        }


        if(GUILayout.Button("Open Scenes"))
        {
            LevelCollection levelCollection = (LevelCollection)target;
            EditorSceneManager.OpenScene(levelCollection.myScenes[0].ScenePath, OpenSceneMode.Single);
            for (int i = 1; i < levelCollection.myScenes.Count; ++i)
                EditorSceneManager.OpenScene(levelCollection.myScenes[i].ScenePath, OpenSceneMode.Additive);
        }

        if (GUILayout.Button("Export"))
        {
            

        }

        serializedObject.ApplyModifiedProperties();
    }
}
