using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VolumetricLightData : MonoBehaviour
{
    public bool isMainDirectionalLight;
    public bool isVolumetric;
    public bool isFog;
    public float numberOfSamples;
    public float lightPower;
    public float scatteringProbability;
    public float henyeyGreensteinGValue;
}
