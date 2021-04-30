using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Text;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct DirectionalLightData
{
    public int instanceID;
    public Vector3 direction;
    public float r;
    public float g;
    public float b;
    public float intensity;
    public string cubemapName;
    public bool isMainDirectionalLight;
    public bool isVolumetric;
    public bool isFog;
    public float numberOfSamples;
    public float lightPower;
    public float scatteringProbability;
    public float henyeyGreensteinGValue;
}

[System.Serializable]
public struct DirectionalLightCollection
{
    public List<DirectionalLightData> directionalLights;
}

public class ExportDirectionalLight
{
    public static DirectionalLightCollection Export(string aSceneName)
    {
        CubemapReference[] references = GameObject.FindObjectsOfType<CubemapReference>();
        List<DirectionalLightData> directionalLights = new List<DirectionalLightData>();
        //DirectionalLightCollection newLights = new DirectionalLightCollection();
        //CubemapReference reference = GameObject.FindObjectOfType<CubemapReference>();

        for (int i = 0; i < references.Length; ++i)
        {
            //if (references[i] != null)
            //{
                if (references[i].transform.GetChild(0).GetComponent<Light>().type == LightType.Directional)
                {
                    GameObject lightPrefab = references[i].gameObject;
                    VolumetricLightData lightData = lightPrefab.GetComponent<VolumetricLightData>();
                    Light light = lightPrefab.transform.GetComponentInChildren<Light>();
                    DirectionalLightData data = new DirectionalLightData();

                    data.instanceID = lightPrefab.transform.GetInstanceID();
                    data.direction = -lightPrefab.transform.forward;
                    data.r = light.color.r;
                    data.g = light.color.g;
                    data.b = light.color.b;
                    data.intensity = light.intensity;
                    data.cubemapName = references[i].cubeMap.name;
                    data.isMainDirectionalLight = lightData.isMainDirectionalLight;
                    data.isVolumetric = lightData.isVolumetric;
                    data.isFog = lightData.isFog;
                    data.numberOfSamples = lightData.numberOfSamples;
                    data.lightPower = lightData.lightPower;
                    data.scatteringProbability = lightData.scatteringProbability;
                    data.henyeyGreensteinGValue = lightData.henyeyGreensteinGValue;

                    directionalLights.Add(data);
                    //newLights.directionalLights.Add(data);
                }  
            //}
        }

        DirectionalLightCollection lightCollection = new DirectionalLightCollection();
        lightCollection.directionalLights = directionalLights;

        return lightCollection;
    }
}
