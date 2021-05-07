using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct ParentData
{
    public Transform parent;
    public List<Transform> children;
} 

[System.Serializable]
public struct ParentsCollection
{
    public List<ParentData> parents;
}

public class ExportParents
{
    public static ParentsCollection Export(string aSceneName)
    {
        ParentsCollection collection = new ParentsCollection();
        collection.parents = new List<ParentData>();

        Parent[] parents = GameObject.FindObjectsOfType<Parent>();
        foreach(Parent parent in parents)
        {
            if (parent.myChildren.Count == 0)
                continue;

            ParentData data = new ParentData();
            data.children = new List<Transform>();
            data.parent = parent.transform;
            foreach (Transform child in parent.myChildren)
            {
                if(child != null)
                    data.children.Add(child);
            }

            collection.parents.Add(data);
        }

        return collection;
    }
}
