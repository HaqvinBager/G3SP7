using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using UnityEditor;
using UnityEngine.SceneManagement;
using UnityEditor.SceneManagement;

public class ExporterBin
{
    private static string Base { get => "Assets/Generated/"; }

    BinaryWriter binWriter = null;

    public ExporterBin(string aFileName)
    {
        string rootFolder = Base;
        string folder = rootFolder + aFileName;

        if (!Directory.Exists(Application.dataPath + folder))
        {
            Directory.CreateDirectory(rootFolder + aFileName);
        }

        AssetDatabase.Refresh();

        binWriter = new BinaryWriter(new FileStream(folder + "\\" + aFileName + ".bin", FileMode.Create));
    }


    //[MenuItem("Export/Export Bin")]
    public static void Export()
    {
        Scene activeScene = EditorSceneManager.GetActiveScene();
        DirectoryInfo directoryInfo = new DirectoryInfo(activeScene.path);
        string fullName = directoryInfo.Parent.FullName;
        string levelFolder = fullName.Substring(fullName.IndexOf("Assets"), fullName.Length - fullName.IndexOf("Assets"));
        LevelCollection level = AssetDatabase.LoadAssetAtPath<LevelCollection>(levelFolder + "\\" + directoryInfo.Parent.Name + "_Scenes.asset");

        Debug.Log("Exporting " + level.name, level);

        var ids = ExportInstanceID.Export(level.name);
        var transforms = ExportTransform.Export(level.name, ids.Ids);
        var models = ExportModel.Export(level.name, ids.Ids);
        var instancedModels = ExportInstancedModel.Export(level.name);
        var pointLights = ExportPointlights.ExportPointlight(level.name);
        var colliders = ExportCollider.Export(level.name, ids.Ids);
        
        ExporterBin exporter = new ExporterBin(directoryInfo.Parent.Name);
        exporter.binWriter.Write(ids);
        exporter.binWriter.Write(transforms);
        exporter.binWriter.Write(models);
        exporter.binWriter.Write(pointLights);
        exporter.binWriter.Write(colliders);
        exporter.binWriter.Write(instancedModels);
        exporter.binWriter.Close();

        Debug.Log(level.name + " Export Complete <3");
        //var resources = ExportResource.ExportModelAssets("Scene");
        //ExporterBin resourceBin = new ExporterBin("Resources");
        //resourceBin.binWriter.Write(resources.models.Count * 100);

        //foreach (var res in resources.models)
        //{
        //    resourceBin.binWriter.Write(res.path);     
        //}
        //resourceBin.binWriter.Close();
    }
}
