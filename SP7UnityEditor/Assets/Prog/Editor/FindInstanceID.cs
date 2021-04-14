using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

public class FindInstanceID : EditorWindow
{
    [MenuItem("IronWrought/Find Instance ID")]
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
            Scene currentActiveScene = SceneManager.GetActiveScene();
            int count = SceneManager.sceneCount;
            for(int i = 0; i < count; ++i)
            {
                Scene test = SceneManager.GetSceneAt(i);
                SceneManager.SetActiveScene(test);

                Transform[] transforms = GameObject.FindObjectsOfType<Transform>();
                foreach (Transform transform in transforms)
                {
                    if (transform.GetInstanceID() == findInstanceID)
                    {
                        transform.Ping();
                        SceneManager.SetActiveScene(currentActiveScene);
                        Debug.Log("Found: " + transform.name, transform.gameObject);
                        return;
                    }
                }
            }
            SceneManager.SetActiveScene(currentActiveScene);
            Debug.Log("Did not find Instance ID: " + findInstanceID);
        }
    }
}
