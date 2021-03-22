using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Text;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct PointLight
{
    public int instanceID;
    public float range;
    public float r;
    public float g;
    public float b;
    public float intensity;
}
[System.Serializable]
public struct PointLightCollection
{
   public List<PointLight> lights;
}
public class ExportPointlights 
{
    public static PointLightCollection ExportPointlight(string aSceneName)
    {
        Light[] allLights = GameObject.FindObjectsOfType<Light>();
        List<PointLight> pointlights = new List<PointLight>();
       
        for(int i = 0; i < allLights.Length; ++i)
        {
            PointLight lightValue = new PointLight();
            if (allLights[i].transform.parent == null)
                continue;

            if (allLights[i].type != LightType.Point)
                continue;

            lightValue.instanceID = allLights[i].transform.parent.GetInstanceID();
            lightValue.range = allLights[i].range;
            lightValue.r = allLights[i].color.r;
            lightValue.g = allLights[i].color.g;
            lightValue.b = allLights[i].color.b;
            lightValue.intensity = allLights[i].intensity;
            pointlights.Add(lightValue);
        }

        PointLightCollection lightCollection = new PointLightCollection();
        lightCollection.lights = pointlights;

        return lightCollection;
        //Json.ExportToJson(lightCollection, aScene.name);
        
    }
}



//if (!System.IO.Directory.Exists(SavePath))
//    System.IO.Directory.CreateDirectory(SavePath);

//StringBuilder stringBuilder = new StringBuilder();
//stringBuilder.Append(JsonUtility.ToJson(lightCollection));

//string savePath = SavePath + "PointlightLinks_" + aScene.name + ".json";
//System.IO.File.WriteAllText(savePath, stringBuilder.ToString());

//Hämta ut alla pointlights i scenen och deras Range, Color och intensity = DONE
//Spara undan den datan i en Container som vi sedan kan spara i en JSon fil.