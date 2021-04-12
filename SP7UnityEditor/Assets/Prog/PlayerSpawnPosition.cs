using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerSpawnPosition : MonoBehaviour
{
    [Range(0.1f, 0.8f)]
    public float sphereSize = 0.0f;

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.Lerp(Color.cyan, Color.green, 0.5f);
        Gizmos.DrawSphere(transform.position, sphereSize);
    }

}
