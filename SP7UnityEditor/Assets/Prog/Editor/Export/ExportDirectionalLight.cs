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
        //Light[] allLights = GameObject.FindObjectsOfType<Light>();
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
                //Debug.LogError("No Directional Light Prefab found!");        
        }
        
            //Debug.LogError("No Directional Light Prefab found!");


        //for (int i = 0; i < allLights.Length; ++i)
        //{
        //    if (allLights[i].transform.parent == null)
        //        continue;

        //    if (allLights[i].type != LightType.Directional)
        //        continue;

        //    light.directionalLight.instanceID = allLights[i].transform.parent.GetInstanceID();
        //    light.directionalLight.direction = -allLights[i].transform.forward;
        //    light.directionalLight.r = allLights[i].color.r;
        //    light.directionalLight.g = allLights[i].color.g;
        //    light.directionalLight.b = allLights[i].color.b;
        //    light.directionalLight.intensity = allLights[i].intensity;
        //    light.directionalLight.cubemapName = allLights[i].GetComponentInParent<CubemapReference>().cubeMap.name;
        //    break;
        //}

        return newLight;
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
