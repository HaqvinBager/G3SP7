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
        string folder = rootFolder + aFileName + "_Bin";

        if (!Directory.Exists(Application.dataPath + folder))
        {
            Directory.CreateDirectory(rootFolder + aFileName + "_Bin");
        }

        AssetDatabase.Refresh();

        binWriter = new BinaryWriter(new FileStream(folder + "\\" + aFileName + ".bin", FileMode.Create));
    }


    [MenuItem("Export/Export Bin")]
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
        var eventTriggers = ExportEventTrigger.Export(level.name);
        
        //var vertexColors = ExportVertexPaint.Export(level.name, ids.Ids);
        //var directionalLight = ExportDirectionalLight.Export(level.name);
        //var decals = ExportDecals.Export(level.name);
        //var player = ExporterJson.ExportPlayer(level.name);
        //var bluePrints = ExportBluePrint.Export(level.name);
        //var enemies = EnemyExporter.Export(level.name);
        //var parents = ExportParents.Export(level.name);

        ExporterBin exporter = new ExporterBin(directoryInfo.Parent.Name);

        exporter.binWriter.Write(ids);
        exporter.binWriter.Write(transforms);
        exporter.binWriter.Write(models);
        exporter.binWriter.Write(pointLights);
        exporter.binWriter.Write(colliders);
        exporter.binWriter.Write(eventTriggers);
        exporter.binWriter.Write(instancedModels);
        //exporter.binWriter.Write(vertexColors); //TODO
        //exporter.binWriter.Write(directionalLight);
        //exporter.binWriter.Write(decals); // TOOD
        //exporter.binWriter.Write(player);
        //exporter.binWriter.Write(bluePrints); //Leave in JSON for now!
        //exporter.binWriter.Write(enemies); // TODO Manage Children Instance IDs etc.
        //exporter.binWriter.Write(parents);

        exporter.binWriter.Close();


        var resources = ExportResource.ExportModelAssets("Scene");
        ExporterBin resourceBin = new ExporterBin("Resources");
        resourceBin.binWriter.Write(resources.models.Count * 100);

        foreach (var res in resources.models)
        {
            resourceBin.binWriter.Write(res.path);
        }
        resourceBin.binWriter.Close();
        Debug.Log(level.name + " Export Complete <3");
    }
}
