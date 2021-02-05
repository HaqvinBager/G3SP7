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
    public List<int> myGameObjectIDs;
    public List<string> myMaterialNames;
}

public class ExportVertexPaint : Editor
{
    static string targetPath = "Assets\\Generated\\";
    static string polybrushMesh = "PolybrushMesh";

    [MenuItem("Export/Export Vertex Painting")]
    public static void ExtractColorsFromMeshes()
    {
        List<VertexLink> vertexLinks = new List<VertexLink>();

        List<MeshRenderer> meshRenderers = GetAllVertexPaintedMeshRenderers();
        List<string> exportedMeshPaths = GetAllExportedPolyBrushMeshPaths();

        foreach (MeshRenderer renderer in meshRenderers)
        {
            string meshName = renderer.GetComponent<MeshFilter>().sharedMesh.name;
            int startIndex = meshName.LastIndexOf(polybrushMesh) + polybrushMesh.Length;
            int endIndex = meshName.Length;
            string polyMeshID = meshName.Substring(startIndex, endIndex - startIndex);

            bool hasFoundLink = false;
            foreach (VertexLink vertexLink in vertexLinks)
            {
                if (vertexLink.colorsPath == targetPath + "PolybrushColors_" + polyMeshID + "_Bin.bin")
                {
                    if (!vertexLink.myGameObjectIDs.Contains(renderer.gameObject.GetInstanceID()))
                    {
                        vertexLink.myGameObjectIDs.Add(renderer.gameObject.GetInstanceID());
                        hasFoundLink = true;
                        break;
                    }
                }
            }

            if (!hasFoundLink)
            {
                VertexLink newLink = new VertexLink();
                newLink.colorsPath = targetPath + "PolybrushColors_" + polyMeshID + "_Bin.bin";
                newLink.myGameObjectIDs = new List<int>();
                newLink.myGameObjectIDs.Add(renderer.gameObject.GetInstanceID());
                newLink.myMaterialNames = ExtractTexturePathsFromMaterials(renderer.sharedMaterials);
                vertexLinks.Add(newLink);
            }

            // Binary
            Mesh exportedMeshObject = renderer.GetComponent<MeshFilter>().sharedMesh;
            if (int.TryParse(polyMeshID, out int polyMeshIDNumber))
            {
                BinaryWriter bin;

                if (!System.IO.Directory.Exists(targetPath))
                    System.IO.Directory.CreateDirectory(targetPath);

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

                //Ta den FBXen vi kommer använda 

                //Commandline tool 

                bin.Close();
                Debug.Log("Exported PolybrushMesh_" + polyMeshIDNumber + ".", exportedMeshObject);
            }
        }

        // Json
        VertexLinkCollection collection = new VertexLinkCollection();
        collection.links = vertexLinks.ToArray();
        string jsonString = JsonUtility.ToJson(collection);
        string savePath = targetPath + "PolybrushLinks_" + SceneManager.GetActiveScene().name + ".json";
        System.IO.File.WriteAllText(savePath, jsonString);

        AssetDatabase.Refresh();
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

    static List<MeshRenderer> GetAllVertexPaintedMeshRenderers()
    {
        List<string> exportedMeshPaths = GetAllExportedPolyBrushMeshPaths();
        List<MeshRenderer> meshRenderers = new List<MeshRenderer>();
        MeshFilter[] meshFilters = GameObject.FindObjectsOfType<MeshFilter>();
        foreach (MeshFilter filter in meshFilters)
        {
            string meshName = filter.sharedMesh.name;
            if (meshName.Contains(polybrushMesh))
            {
                meshRenderers.Add(filter.GetComponent<MeshRenderer>());
            }
        }
        return meshRenderers;
    }

    static List<string> ExtractTexturePathsFromMaterials(Material[] materials)
    {
        List<string> texturePaths = new List<string>();
        MaterialProperty[] matProperty = MaterialEditor.GetMaterialProperties(materials);
        for (int i = 0; i < matProperty.Length; ++i)
        {
            if (!matProperty[i].displayName.Contains("Base"))
            {
                Debug.Log(matProperty[i].displayName);
                if (matProperty[i].textureValue != null)
                {
                    FileInfo fileInfo = new FileInfo(AssetDatabase.GetAssetPath(matProperty[i].textureValue));
                    if (fileInfo.Name.Contains("_c")) 
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