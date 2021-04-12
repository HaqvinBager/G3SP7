using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerSpawnPosition : MonoBehaviour
{

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.Lerp(Color.cyan, Color.green, 0.5f);
        Gizmos.DrawSphere(transform.position, 0.5f);
    }

}
