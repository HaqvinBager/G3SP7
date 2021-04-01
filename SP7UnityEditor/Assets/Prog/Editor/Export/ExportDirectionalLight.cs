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
}

[System.Serializable]
public struct DirectionalLight
{
    public DirectionalLightData directionalLight;
}

public class ExportDirectionalLight
{
    public static DirectionalLight Export(string aSceneName)
    {
        DirectionalLight newLight = new DirectionalLight();
        CubemapReference reference = GameObject.FindObjectOfType<CubemapReference>();

        if (reference != null)
        {
            if (reference.transform.GetChild(0).GetComponent<Light>().type == LightType.Directional)
            {
                GameObject lightPrefab = reference.gameObject;
                Light light = lightPrefab.transform.GetComponentInChildren<Light>();

                newLight.directionalLight.instanceID = lightPrefab.transform.GetInstanceID();
                newLight.directionalLight.direction = -lightPrefab.transform.forward;
                newLight.directionalLight.r = light.color.r;
                newLight.directionalLight.g = light.color.g;
                newLight.directionalLight.b = light.color.b;
                newLight.directionalLight.intensity = light.intensity;
                newLight.directionalLight.cubemapName = reference.cubeMap.name;
            }  
        }

        return newLight;
    }
}
