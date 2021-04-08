using System.Collections;
using System.Collections.Generic;
using UnityEngine;



[System.Serializable]
public struct EnemyCollection
{
    public List<Enemy> enemies;
}

public class EnemyExporter 
{
    public static EnemyCollection Export(string aSceneName)
    {

        EnemyCollection collection = new EnemyCollection();
        collection.enemies = new List<Enemy>();

        EnemySettings[] allEnemies = GameObject.FindObjectsOfType<EnemySettings>();
        foreach(EnemySettings enemy in allEnemies)
        {
            enemy.settings.instanceID = enemy.transform.GetInstanceID();
            collection.enemies.Add(enemy.settings);
        }

        return collection;
    }
}
