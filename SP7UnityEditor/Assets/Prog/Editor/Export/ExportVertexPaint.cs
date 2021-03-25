using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.IO;
using UnityEngine.SceneManagement;

[System.Serializable]
public class VertexLink
{
    public int assetID;
    public List<int> myTransformIDs;
    public List<string> myMaterialNames;
}

[System.Serializable]
public class VertexColorCollection
{
    public List<VertexLink> vertexColors;
}

public class ExportVertexPaint : Editor
{
    static string targetPath = "Assets\\Generated\\VertexColors\\";
    static string polybrushMesh = "PolybrushMesh";

    [MenuItem("Tools/Select Paintable %k")]
    public static void SelectPaintable()
    {
        var objects = Selection.objects;

        List<GameObject> paintableObjects = new List<GameObject>();
        foreach(var obj in objects)
        {
            GameObject gameObject = obj as GameObject;
            if (gameObject.TryGetComponent(out MeshRenderer renderer))
            {
                if (renderer.sharedMaterials.Length == 1)
                {
                    paintableObjects.Add(gameObject);
                }
            }
        }

        Selection.activeGameObject = paintableObjects[0];
        //Selection.
        //Selection.objects = objects;
    }

    [MenuItem("Tools/Enable VertexPaint on Selected", true)]
    public static bool IsValid()
    {
        int correctItems = 0;
        GameObject[] selectedObjects = Selection.gameObjects;
        foreach (GameObject selectedObject in selectedObjects)
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
        foreach (GameObject selectedObject in selectedObjects)
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
    public static VertexColorCollection Export(string aSceneName, List<int> validExportIds)
    {
        //List<GameObject> exportedVertexPaintObjects = new List<GameObject>();
        //List<VertexLink> vertexLinks = new List<VertexLink>();
        PolybrushFBX[] vertexPaintedObjects = GetAllVertexPaintedObjects();

        VertexColorCollection collection = new VertexColorCollection();
        collection.vertexColors = new List<VertexLink>();


        foreach (PolybrushFBX polyBrushObject in vertexPaintedObjects)
        {
            if (polyBrushObject.transform.parent == null)
                continue;

            string meshName = polyBrushObject.GetComponent<MeshFilter>().sharedMesh.name;
            if (!meshName.Contains(polybrushMesh))
            {
                Debug.LogError("This Object has not yet been Painted on. Skipping it!", polyBrushObject.gameObject);
                continue;
            }

            int startIndex = meshName.LastIndexOf(polybrushMesh) + polybrushMesh.Length;
            int endIndex = meshName.Length;
            string polyMeshID = meshName.Substring(startIndex, endIndex - startIndex);
            string materialName = polyBrushObject.GetComponent<MeshRenderer>().sharedMaterial.name;
            string filePath = targetPath + "VertexColors_" + polyBrushObject.GetInstanceID().ToString() + "_Bin.bin";

            Object binAsset = null;
            if (polyBrushObject.TryGetComponent(out MeshFilter meshFilter))
            {
                Mesh exportedMeshObject = meshFilter.sharedMesh;
                //if (int.TryParse(polyMeshID, out int polyMeshIDNumber))
                //{
                    if (!Directory.Exists(targetPath))
                        Directory.CreateDirectory(targetPath);

                    FileStream file = new FileStream(filePath, FileMode.Create);
                    BinaryWriter bin = new BinaryWriter(file);

                    bin.Write(polyBrushObject.GetInstanceID());
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
                    AssetDatabase.Refresh();
                    //file.Close();

                    binAsset = AssetDatabase.LoadAssetAtPath<Object>(filePath);
                //}
            }
            else
            {
                Debug.LogWarning("Expected to find a MeshFilter Component on " + polyBrushObject.name, polyBrushObject.gameObject);
            }


            if (validExportIds.Contains(polyBrushObject.transform.parent.GetInstanceID()))
            {
                VertexLink link = collection.vertexColors.Find(e => e.assetID.Equals(binAsset.GetInstanceID()));
                if (link == null)
                {
                    link = new VertexLink();
                    link.myTransformIDs = new List<int>();
                    link.assetID = binAsset.GetInstanceID();//targetPath + "VertexColors_" + polyMeshID + "_Bin.bin";
                    link.myMaterialNames = ExtractTexturePathsFromMaterials(polyBrushObject.GetComponent<MeshRenderer>().sharedMaterials);
                    collection.vertexColors.Add(link);
                }
                link.myTransformIDs.Add(polyBrushObject.transform.parent.GetInstanceID());
            }

        }


        // Json
       // VertexColorCollection collection = new VertexColorCollection();
        //collection.vertexColors = vertexLinks;
        return collection;
        //Json.ExportToJson(collection, aScene.name);
        //string jsonString = JsonUtility.ToJson(collection);

        //if (!File.Exists("Assets\\Generated"))
        //    Directory.CreateDirectory(Directory.GetCurrentDirectory() + "\\" + targetPath);
        ////File.Create(targetPath);

        //string savePath = targetPath + "PolybrushLinks_" + aScene.name + ".json";


        //File.WriteAllText(savePath, jsonString);

        //  return exportedVertexPaintObjects;
    }
    //BinaryWriter bin = new BinaryWriter(new FileStream(targetPath + "VertexColors_" + polyMeshID + "_Bin.bin", FileMode.Create));


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