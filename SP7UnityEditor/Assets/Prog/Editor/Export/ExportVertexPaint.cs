using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.IO;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct VertexColorData
{
    public int id;
    public int colorCount;
    public Vector3[] colors;
    public int vertexCount;
    public Vector3[] vertices;
}

[System.Serializable]
public class VertexLink
{
    public int vertexColorsID;
    public List<int> instanceIDs;
    public List<string> materialNames;
}

[System.Serializable]
public class VertexColorCollection
{
    public List<VertexLink> vertexColors;
}

public class ExportVertexPaint : Editor
{
    static string targetPath = "Assets\\Generated\\";
    static string polybrushMesh = "PolybrushMesh";

    [MenuItem("Tools/Select Paintable %k")]
    public static void SelectPaintable()
    {
        var objects = Selection.objects;

        List<GameObject> paintableObjects = new List<GameObject>();
        foreach (var obj in objects)
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

    public static VertexColorCollection Export(string aSceneName, List<int> validExportIds)
    {
        PolybrushFBX[] vertexPaintedObjects = GameObject.FindObjectsOfType<PolybrushFBX>();
        if (vertexPaintedObjects.Length == 0)
            return new VertexColorCollection();

        VertexColorCollection collection = new VertexColorCollection();
        collection.vertexColors = new List<VertexLink>();

        //string baseSceneName = aSceneName.Substring(0, aSceneName.LastIndexOf('-') + 2);

        string folderName = Directory.GetParent(SceneManager.GetActiveScene().path).Name;

        string path = targetPath + folderName + "\\VertexColors\\";

        if (!Directory.Exists(path))
            Directory.CreateDirectory(path);

        //Expected Path
        //Generated/Level_VertexPaint//VertexColors.bin

        //Vad är ett unikt ID För denna? Kanske Shared Mesh' ID? <-- yes


        List<VertexColorData> colorData = new List<VertexColorData>();
        //AssetDatabase.LoadAssetAtPath<Object>(binFile);

        foreach (PolybrushFBX polyBrushObject in vertexPaintedObjects)
        {
            if (polyBrushObject.transform.parent == null)
                continue;

            MeshFilter meshFilter = polyBrushObject.GetComponent<MeshFilter>();

            string meshName = meshFilter.sharedMesh.name;
            if (!meshName.Contains(polybrushMesh))
            {
                Debug.LogError("This Object has not yet been Painted on. Skipping it!", polyBrushObject.gameObject);
                //continue;
            }

            //bool isLiterallyVertexPainted = false;
            //if (polyBrushObject.TryGetComponent(out MeshFilter filter))
            //{
            //    isLiterallyVertexPainted = filter.sharedMesh.name.Contains("PolybrushMesh");
            //}

            //if (isLiterallyVertexPainted)
            //{
            //if (polyBrushObject.TryGetComponent(out MeshFilter meshFilter))
            //{
            VertexColorData data = new VertexColorData();
            data.id = meshFilter.sharedMesh.GetInstanceID();
            data.colorCount = meshFilter.sharedMesh.colors.Length;
            data.colors = new Vector3[meshFilter.sharedMesh.colors.Length];
            for (int i = meshFilter.sharedMesh.colors.Length - 1; i > -1; --i)
            {
                data.colors[i].x = meshFilter.sharedMesh.colors[i].r;
                data.colors[i].y = meshFilter.sharedMesh.colors[i].g;
                data.colors[i].z = meshFilter.sharedMesh.colors[i].b;

            }
            data.vertexCount = meshFilter.sharedMesh.vertices.Length;
            data.vertices = meshFilter.sharedMesh.vertices;
            colorData.Add(data);

            VertexLink link = collection.vertexColors.Find(e => e.vertexColorsID.Equals(meshFilter.sharedMesh.GetInstanceID()));
            if (link == null)
            {
                link = new VertexLink();
                link.instanceIDs = new List<int>();
                link.vertexColorsID = meshFilter.sharedMesh.GetInstanceID(); //binAsset.GetInstanceID();//targetPath + "VertexColors_" + polyMeshID + "_Bin.bin";
                link.materialNames = ExtractTexturePathsFromMaterials(polyBrushObject.GetComponent<MeshRenderer>().sharedMaterials);
                collection.vertexColors.Add(link);

            }
            link.instanceIDs.Add(polyBrushObject.transform.parent.GetInstanceID());
            //}
            //else
            //{
            //    Debug.LogWarning("Expected to find a MeshFilter Component on " + polyBrushObject.name, polyBrushObject.gameObject);
            //}
            //}
        }

        string binFile = path + aSceneName + "_VertexColors.bin";
        FileStream file = new FileStream(binFile, FileMode.Create);
        BinaryWriter writer = new BinaryWriter(file);
        writer.Write(colorData.Count);
        for (int i = 0; i < colorData.Count; ++i)
        {
            writer.Write(colorData[i].id);
            writer.Write(colorData[i].colorCount);
            writer.Write(colorData[i].colors);
            writer.Write(colorData[i].vertexCount);
            writer.Write(colorData[i].vertices);
        }
        writer.Close();

        return collection;
    }

    private static string BinFileName(PolybrushFBX polyBrushObject, string meshName)
    {
        int startIndex = meshName.LastIndexOf(polybrushMesh) + polybrushMesh.Length;
        int endIndex = meshName.Length;
        string polyMeshID = meshName.Substring(startIndex, endIndex - startIndex);
        string materialName = polyBrushObject.GetComponent<MeshRenderer>().sharedMaterial.name;
        string binFileName = "VertexColors_" + polyBrushObject.GetInstanceID().ToString() + "_Bin.bin";
        return binFileName;
    }

    //Ta reda på hur många ColorBin Filer kommer skapas för denna scen (Även om scenen tillhör samma Level)
    private static void ExportVertexColorBin(PolybrushFBX polyBrushObject, string filePath, Mesh exportedMeshObject)
    {


        //if (!Directory.Exists(targetPath))
        //    Directory.CreateDirectory(targetPath);

        //FileStream file = new FileStream(filePath, FileMode.Create);
        //BinaryWriter bin = new BinaryWriter(file);

        //bin.Write(polyBrushObject.GetInstanceID());
        //bin.Write(exportedMeshObject.colors.Length);

        //Vector3[] colorsRGB = new Vector3[exportedMeshObject.colors.Length];
        //for (int i = exportedMeshObject.colors.Length - 1; i > -1; --i)
        //{
        //    colorsRGB[i].x = exportedMeshObject.colors[i].r;
        //    colorsRGB[i].y = exportedMeshObject.colors[i].g;
        //    colorsRGB[i].z = exportedMeshObject.colors[i].b;
        //}
        //bin.Write(colorsRGB);

        //Vector3[] vertexPositions = exportedMeshObject.vertices;
        //bin.Write(vertexPositions.Length);
        //bin.Write(vertexPositions);
        //bin.Close();
        //AssetDatabase.Refresh();
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