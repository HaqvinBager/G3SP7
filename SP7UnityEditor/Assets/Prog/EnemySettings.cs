using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct Enemy
{
    [HideInInspector]
    public int instanceID;

    public float speed;
    public float radius;
    public float health;
    public float attackDistance;
    public List<Transform> points;
}

public class EnemySettings : MonoBehaviour
{
    public Enemy settings = new Enemy { speed = 2.0f, radius = 10.0f,  health = 10.0f, attackDistance = 2.0f };  
}
