using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Text;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct SLight
{
    public int instanceID;
    public float range;
    public float r;
    public float g;
    public float b;
    public float intensity;
}
[System.Serializable]
public struct SLightCollection
{
   public List<SLight> lights;
}
public class ExportPointlights 
{
    private static string SavePath
    {
        get => System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\";
    }

    public static void ExportPointlight(Scene aScene)
    {
        UnityEngine.Light[] allLights = GameObject.FindObjectsOfType<UnityEngine.Light>();
        // Debug.Log("Light" + lights.Length.ToString());
        List<SLight> pointlights = new List<SLight>();
       
        for(int i = 0; i < allLights.Length; ++i)
        {
            SLight lightValue = new SLight();
            lightValue.instanceID = allLights[i].transform.GetInstanceID();
            lightValue.range = allLights[i].range;
            lightValue.r = allLights[i].color.r;
            lightValue.g = allLights[i].color.g;
            lightValue.b = allLights[i].color.b;
            lightValue.intensity = allLights[i].intensity;
            pointlights.Add(lightValue);
        }

        SLightCollection lightCollection = new SLightCollection();
        lightCollection.lights = pointlights;
        
        if (!System.IO.Directory.Exists(SavePath))
            System.IO.Directory.CreateDirectory(SavePath);

        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.Append(JsonUtility.ToJson(lightCollection));

        string savePath = SavePath + "PointlightLinks_" + aScene.name + ".json";
        System.IO.File.WriteAllText(savePath, stringBuilder.ToString());
        AssetDatabase.Refresh();
    }
}


//Hämta ut alla pointlights i scenen och deras Range, Color och intensity = DONE
//Spara undan den datan i en Container som vi sedan kan spara i en JSon fil.