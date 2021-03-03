using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;
using System.Text;

[System.Serializable]
public struct SIDScene
{
    public List<int> instanceIDs;
}

public class ExportViewer : EditorWindow
{

    static ExportViewer view;
   // [MenuItem("Export/View")]
    static void OpenWindow()
    {
        view = EditorWindow.CreateWindow<ExportViewer>();



        //StringBuilder str = Exporter.ExportAScene(SceneManager.GetActiveScene());
        //Debug.Log(str);
    }

    private void OnGUI()
    {
        GameObject[] gameObjects = GameObject.FindObjectsOfType<GameObject>();
        SIDScene scene = new SIDScene();
        scene.instanceIDs = new List<int>(gameObjects.Length);
        foreach (GameObject gameObject in gameObjects)
        {
            scene.instanceIDs.Add(gameObject.transform.GetInstanceID());
        }

        StringBuilder json = new StringBuilder(JsonUtility.ToJson(scene));
        EditorGUILayout.TextArea(json.ToString());

    }


}
