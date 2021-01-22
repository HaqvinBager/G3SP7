using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName ="LD/Level Collection")]
public class LevelCollection : ScriptableObject
{
    public List<SceneReference> myScenes = new List<SceneReference>();
}
