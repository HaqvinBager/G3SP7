using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct Enemy
{
    [HideInInspector]
    public int instanceID;

    public float speed;
    public float distance;
    public float radius;
    public float health;
    public List<Transform> points;
}

public class EnemySettings : MonoBehaviour
{
    public Enemy settings = new Enemy { speed = 10.0f, distance = 0.0f, radius = 0.0015f,  health = 10.0f };  
}
