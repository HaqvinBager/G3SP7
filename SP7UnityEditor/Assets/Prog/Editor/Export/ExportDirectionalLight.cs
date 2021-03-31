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
        Light[] allLights = GameObject.FindObjectsOfType<Light>();
        DirectionalLight light = new DirectionalLight(); 

        for (int i = 0; i < allLights.Length; ++i)
        {
            if (allLights[i].transform.parent == null)
                continue;

            if (allLights[i].type != LightType.Directional)
                continue;

            light.directionalLight.instanceID = allLights[i].transform.parent.GetInstanceID();
            light.directionalLight.direction = -allLights[i].transform.forward;
            light.directionalLight.r = allLights[i].color.r;
            light.directionalLight.g = allLights[i].color.g;
            light.directionalLight.b = allLights[i].color.b;
            light.directionalLight.intensity = allLights[i].intensity;
            light.directionalLight.cubemapName = allLights[i].GetComponentInParent<CubemapReference>().cubeMap.name;
            break;
        }

        return light;
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
