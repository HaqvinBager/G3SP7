using UnityEngine;
using System.Text;
using UnityEditor;

/*
{
"Scene" : "aSceneName",


 
 
*/


public static class Json
{
    static string mySceneName = "";
    static StringBuilder myCurrentExportJson = new StringBuilder();

    public static void BeginExport(string aSceneName)
    {
        myCurrentExportJson.Clear();
        myCurrentExportJson.AppendLine("{");
        myCurrentExportJson.Append("\"Scene\" : ");
        myCurrentExportJson.AppendLine("\"" + aSceneName + "\",");
        mySceneName = aSceneName;
    }

    public static void AddToExport<T>(T data/*, string aSceneName*/)
    {
        StringBuilder localBuilder = new StringBuilder();
        localBuilder.Append(JsonUtility.ToJson(data));

        //localBuilder.Remove(0, 1);
        localBuilder.Insert(0, "\"" + data.GetType().Name + "\"" + " : ");

        //localBuilder.Remove(localBuilder.Length - 1, 1);
        //localBuilder.Insert(localBuilder.Length - 1, "\n},");

        myCurrentExportJson.AppendLine(localBuilder.ToString());
        myCurrentExportJson.Append(',');
        localBuilder.Clear();
    }

    public static void EndExport(string aSceneName)
    {
        if (myCurrentExportJson[myCurrentExportJson.Length - 1] == ',')
            myCurrentExportJson.Remove(myCurrentExportJson.Length - 1, 1);

        myCurrentExportJson.Append("\n}");

        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\";
        if (!System.IO.Directory.Exists(savePath))
            System.IO.Directory.CreateDirectory(savePath);

        savePath += aSceneName + ".json";
        System.IO.File.WriteAllText(savePath, myCurrentExportJson.ToString());
        myCurrentExportJson.Clear();
    }


    /// <summary>
    /// Exports T to a JsonFile (only if it has the System.Serializable Attribue
    /// Example:
    /// <para>T = InstanceIDs </para>
    /// <para>Scene Name = "Level_1" </para>
    /// <para>Creates a Json file With the name: "Level_1_InstanceIDs.json"</para>
    /// </summary>
    public static void ExportToJson<T>(T someDataToExport, string aSceneName) 
    {
        StringBuilder jsonString = new StringBuilder();
        jsonString.Append(JsonUtility.ToJson(someDataToExport));

        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\";
        if (!System.IO.Directory.Exists(savePath))
            System.IO.Directory.CreateDirectory(savePath);

        savePath += aSceneName + "_" + someDataToExport.GetType().Name + ".json";
        System.IO.File.WriteAllText(savePath, jsonString.ToString());
    }

    public static bool TryIsValidExport<T>(T obj, out GameObject objectWithInstanceID, bool aIsStatic = false) where T : Component
    {
        objectWithInstanceID = PrefabUtility.GetOutermostPrefabInstanceRoot(obj);

        if (objectWithInstanceID == null)
            return false;

        if (objectWithInstanceID.isStatic != aIsStatic)
            return false;

        var prefabAssetType = PrefabUtility.GetPrefabAssetType(objectWithInstanceID);
        if (PrefabUtility.GetPrefabAssetType(objectWithInstanceID) != PrefabAssetType.Regular &&
            PrefabUtility.GetPrefabAssetType(objectWithInstanceID) != PrefabAssetType.Variant)
            return false;

        //switch (prefabAssetType)
        //{
        //    case PrefabAssetType.Regular:
        //        if (objectWithInstanceID.name.Contains("BP_"))
        //        {

        //        }
        //        break;
        //    case PrefabAssetType.Variant:
        //        if (objectWithInstanceID.name.Contains("BP_"))
        //        {

        //        }
        //        break;
        //    case PrefabAssetType.Model:

        //        break;
        //    case PrefabAssetType.NotAPrefab:

        //        break;
        //    case PrefabAssetType.MissingAsset:

        //        break;
        //}
        return true;
    }


}
