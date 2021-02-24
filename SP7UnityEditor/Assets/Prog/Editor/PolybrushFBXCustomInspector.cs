using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(PolybrushFBX))]
public class PolybrushFBXCustomInspector : Editor
{
    public override void OnInspectorGUI()
    {
        //base.OnInspectorGUI();

        PolybrushFBX instance = target as PolybrushFBX;
        //instance.originalFBXGUID

        string fbxGuid = serializedObject.FindProperty("originalFBXGUID").stringValue;
        string fbxPath = AssetDatabase.GUIDToAssetPath(fbxGuid);
        GameObject fbxGameObject = AssetDatabase.LoadAssetAtPath<GameObject>(fbxPath);

        EditorGUILayout.ObjectField("FBX", fbxGameObject, typeof(GameObject), false);




    }
}
