using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.IO;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct VertexLinkCollection
{ 
    public VertexLink[] links;
}

[System.Serializable]
public struct VertexLink
{
    public string colorsPath;
    public List<int> myTransformIDs;
    public List<string> myMaterialNames;
}

public class ExportVertexPaint : Editor
{
    static string targetPath = "Assets\\Generated\\";
    static string polybrushMesh = "PolybrushMesh";

    [MenuItem("Tools/Enable VertexPaint on Selected", true)]
    public static bool IsValid()
    {
        int correctItems = 0;
        GameObject[] selectedObjects = Selection.gameObjects;
        foreach(GameObject selectedObject in selectedObjects)
        {
            if (selectedObject != null)
            {
                if (selectedObject.GetComponent<PolybrushFBX>() != null)
                {
                    return false;
                }


                if (selectedObject.TryGetComponent(out MeshRenderer renderer))
                {
                    if (renderer.sharedMaterials.Length == 1)
                    {
                        if (selectedObject.TryGetComponent(out MeshFilter filter))
                        {
                            if (!filter.sharedMesh.name.Contains("PolybrushMesh"))
                            {
                                correctItems++;
                            }
                        }

                    }
                }
            }
        }
        return correctItems == selectedObjects.Length;
        //return true;
    }

    [MenuItem("Tools/Enable VertexPaint on Selected")]
    static void EnableVertexPaint()
    {
        GameObject[] selectedObjects = Selection.gameObjects;
        foreach(GameObject selectedObject in selectedObjects)
        {
            if (selectedObject.TryGetComponent(out MeshFilter filter))
            {
                PolybrushFBX fbxSaver = Undo.AddComponent<PolybrushFBX>(selectedObject);
                string fbxPath = AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(selectedObject.GetComponent<MeshFilter>().sharedMesh));
                string guid = AssetDatabase.AssetPathToGUID(fbxPath);
                fbxSaver.originalFBXGUID = guid;

                selectedObject.GetComponent<MeshRenderer>().sharedMaterial = AssetDatabase.LoadAssetAtPath<Material>("Assets/VertexPaintMaterials/VP_Material_Base.mat");
            }
        }
    }

    //[MenuItem("Export/Export Vertex Painting")]
    public static List<GameObject> ExportVertexPainting(Scene aScene)
    {
        List<GameObject> exportedVertexPaintObjects = new List<GameObject>();
        List<VertexLink> vertexLinks = new List<VertexLink>();
        PolybrushFBX[] vertexPaintedObjects = GetAllVertexPaintedObjects();
      
        foreach (PolybrushFBX polyBrushObject in vertexPaintedObjects)
        {
            string meshName = polyBrushObject.GetComponent<MeshFilter>().sharedMesh.name;

            if (!meshName.Contains(polybrushMesh))
            {
                Debug.LogError("This Object has not yet been Painted on. Skipping it!", polyBrushObject.gameObject);
                continue;
                //return new List<GameObject>();
            }

            int startIndex = meshName.LastIndexOf(polybrushMesh) + polybrushMesh.Length;
            int endIndex = meshName.Length;
            string polyMeshID = meshName.Substring(startIndex, endIndex - startIndex);

            bool hasFoundLink = false;
            foreach (VertexLink vertexLink in vertexLinks)
            {
                if (vertexLink.colorsPath == targetPath + "PolybrushColors_" + polyMeshID + "_Bin.bin")
                {
                    GameObject prefabRoot = PrefabUtility.GetOutermostPrefabInstanceRoot(polyBrushObject);
                    if (!vertexLink.myTransformIDs.Contains(prefabRoot.transform.GetInstanceID()))
                    {
                        vertexLink.myTransformIDs.Add(prefabRoot.transform.GetInstanceID());

                        exportedVertexPaintObjects.Add(polyBrushObject.gameObject);
                        hasFoundLink = true;
                        break;
                    }
                }
            }

            if (!hasFoundLink)
            {
                VertexLink newLink = new VertexLink();
                GameObject prefabRoot = PrefabUtility.GetOutermostPrefabInstanceRoot(polyBrushObject);
              //  prefabRoot.Ping();
                newLink.colorsPath = targetPath + "PolybrushColors_" + polyMeshID + "_Bin.bin";
                newLink.myTransformIDs = new List<int>();
                newLink.myTransformIDs.Add(prefabRoot.transform.GetInstanceID());
                exportedVertexPaintObjects.Add(polyBrushObject.gameObject);
                newLink.myMaterialNames = ExtractTexturePathsFromMaterials(polyBrushObject.GetComponent<MeshRenderer>().sharedMaterials);
                vertexLinks.Add(newLink);
            }

            // Binary
            //GameObject originalFBX = AssetDatabase.LoadAssetAtPath<GameObject>(AssetDatabase.GUIDToAssetPath(polyBrushObject.originalFBXGUID));
            //originalFBX.GetComponent<MeshFilter>().sharedMesh.Ping();

            if(polyBrushObject.TryGetComponent(out MeshFilter meshFilter))
            {
                Mesh exportedMeshObject = meshFilter.sharedMesh;
                //AssetDatabase.LoadAssetAtPath<Mesh>(AssetDatabase.GUIDToAssetPath(polyBrushObject.originalFBXGUID));         
                if (int.TryParse(polyMeshID, out int polyMeshIDNumber))
                {
                    BinaryWriter bin;
                    if (!Directory.Exists(targetPath))
                        Directory.CreateDirectory(targetPath);

                    bin = new BinaryWriter(new FileStream(targetPath + "PolybrushColors_" + polyMeshID + "_Bin.bin", FileMode.Create));
                    bin.Write(polyMeshIDNumber);
                    bin.Write(exportedMeshObject.colors.Length);

                    Vector3[] colorsRGB = new Vector3[exportedMeshObject.colors.Length];
                    for (int i = exportedMeshObject.colors.Length - 1; i > -1; --i)
                    {
                        colorsRGB[i].x = exportedMeshObject.colors[i].r;
                        colorsRGB[i].y = exportedMeshObject.colors[i].g;
                        colorsRGB[i].z = exportedMeshObject.colors[i].b;
                    }
                    bin.Write(colorsRGB);

                    Vector3[] vertexPositions = exportedMeshObject.vertices;
                    bin.Write(vertexPositions.Length);
                    bin.Write(vertexPositions);
                    bin.Close();
                }
            }
            else
            {
                Debug.LogWarning("Expected to find a MeshFilter Component on " + polyBrushObject.name, polyBrushObject.gameObject);
            }
    
        }

        // Json
        VertexLinkCollection collection = new VertexLinkCollection();
        collection.links = vertexLinks.ToArray();
        string jsonString = JsonUtility.ToJson(collection);

        if (!File.Exists("Assets\\Generated"))
            Directory.CreateDirectory(Directory.GetCurrentDirectory() +  "\\" + targetPath);
            //File.Create(targetPath);

        string savePath = targetPath + "PolybrushLinks_" + aScene.name + ".json";


        File.WriteAllText(savePath, jsonString);

        return exportedVertexPaintObjects;
    }

    static List<string> GetAllExportedPolyBrushMeshPaths()
    {
        string[] allAssetPaths = AssetDatabase.GetAllAssetPaths();
        List<string> exportedMeshPaths = new List<string>();
        foreach (string assetPath in allAssetPaths)
        {
            if (assetPath.Contains("ExportedMeshes/"))
            {
                exportedMeshPaths.Add(assetPath);
            }
        }
        return exportedMeshPaths;
    }

    static PolybrushFBX[] GetAllVertexPaintedObjects()
    {
        //List<string> exportedMeshPaths = GetAllExportedPolyBrushMeshPaths();
        //List<PolybrushFBX> meshRenderers = new List<PolybrushFBX>();
        //MeshFilter[] meshFilters = GameObject.FindObjectsOfType<PolybrushFBX>();
        //foreach (MeshFilter filter in meshFilters)
        //{
        //    string meshName = filter.sharedMesh.name;
        //    if (meshName.Contains(polybrushMesh))
        //    {
        //        meshRenderers.Add(filter.GetComponent<MeshRenderer>());
        //    }
        //}
        //return PolybrushFBX;
        return GameObject.FindObjectsOfType<PolybrushFBX>();

    }

    static List<string> ExtractTexturePathsFromMaterials(Material[] materials)
    {
        List<string> texturePaths = new List<string>();
        MaterialProperty[] matProperty = MaterialEditor.GetMaterialProperties(materials);
        for (int i = 0; i < matProperty.Length; ++i)
        {
            if (!matProperty[i].displayName.Contains("Base"))
            {
                if (matProperty[i].textureValue != null)
                {
                    FileInfo fileInfo = new FileInfo(AssetDatabase.GetAssetPath(matProperty[i].textureValue));

                    if (fileInfo.Name.ToLower().Contains("_c")) 
                    {
                        if (!texturePaths.Contains(fileInfo.Directory.Name))
                        {
                            texturePaths.Add(fileInfo.Directory.Name);
                            continue;
                        }
                    } 
                }
            }
        }
        return texturePaths;
    }
}