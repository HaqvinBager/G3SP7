using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode]
public class PrefabProtector : MonoBehaviour
{

    private void OnValidate()
    {
       
    }

    private void Update()
    {
        if (transform.childCount > 0)
        {
            if (transform.GetChild(0).hasChanged)
            {
                transform.GetChild(0).localPosition = Vector3.zero;
                transform.GetChild(0).localRotation = Quaternion.identity;
                transform.GetChild(0).localScale = Vector3.one;
                transform.GetChild(0).hasChanged = false;
            }
        }

        if (TryGetComponent(out MeshCollider collider))
        {
            collider.sharedMesh = GetComponentInChildren<MeshFilter>().sharedMesh;
        }
    }
}
