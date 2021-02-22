using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;


[System.Serializable]
class FBXLink
{
    public string fbxPath;
    public List<int> instanceIDs;
}

[System.Serializable]
class FBXCollection
{
    public List<FBXLink> fbxLinks;
}

public class ExportFbx
{
    public static void Export(Scene aScene)
    {
        FBXCollection fbxLinks = new FBXCollection();
        fbxLinks.fbxLinks = new List<FBXLink>();

        Renderer[] allrenderers = GameObject.FindObjectsOfType<Renderer>();

        foreach(Renderer renderer in allrenderers)
        {
            if (renderer.GetComponent<PolybrushFBX>() != null)
                continue;

            GameObject prefabParent = PrefabUtility.GetOutermostPrefabInstanceRoot(renderer);

            if(renderer.TryGetComponent(out MeshFilter meshFilter))
            {


                Object source = PrefabUtility.GetCorrespondingObjectFromOriginalSource(meshFilter.sharedMesh);
                if (source == null)
                    continue;

                string fbxPath = AssetDatabase.GetAssetPath(source);
                FBXLink link = fbxLinks.fbxLinks.Find(e => e.fbxPath.Equals(fbxPath));

                if(link == null)
                {
                    link = new FBXLink();
                    link.fbxPath = fbxPath;
                    link.instanceIDs = new List<int>();
                    fbxLinks.fbxLinks.Add(link);
                }

                link.instanceIDs.Add(prefabParent.transform.GetInstanceID());   
            }
        }

        Json.ExportToJson(fbxLinks, aScene.name);
    }
}
