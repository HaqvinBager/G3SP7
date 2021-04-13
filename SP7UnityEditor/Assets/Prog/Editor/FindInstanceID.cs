using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class FindInstanceID : EditorWindow
{
    [MenuItem("Help/Find Instance ID")]
    public static void Find()
    {
        EditorWindow.CreateWindow<FindInstanceID>();
    }


    int findInstanceID = 0;
    private void OnGUI()
    {
        findInstanceID = EditorGUILayout.IntField("Instance ID: ", findInstanceID);
        if (GUILayout.Button("Find"))
        {
            Transform[] transforms = GameObject.FindObjectsOfType<Transform>();
            foreach (Transform transform in transforms)
            {
                if(transform.GetInstanceID() == findInstanceID)
                {
                    transform.Ping();
                    return;
                }
            }

            Debug.Log("Did not find Instance ID: " + findInstanceID);
        }



    }
}
