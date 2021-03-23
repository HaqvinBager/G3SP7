using UnityEngine;
using System.Text;
using UnityEditor;


[System.Serializable]
public struct SceneName
{
    public string name;
}


public static class Json
{
    static string mySceneName = "";
    static string myFolderName = "";
    static StringBuilder myCurrentExportJson = new StringBuilder();

    public static void BeginExport(string baseSceneName)
    {
        myCurrentExportJson.Clear();
        myCurrentExportJson.AppendLine("{");
        myCurrentExportJson.Append("\"Scenes\" : [\n");
        //myCurrentExportJson.AppendLine("\"" + aSceneName  + "\"" + " : " + "\"" + aSceneName + "\",");

        //myCurrentExportJson.AppendLine("\"" + aSceneName + "\",");
        mySceneName = baseSceneName;
    }

    public static void BeginScene(string aSceneName)
    {
        myCurrentExportJson.AppendLine("{");
        //myCurrentExportJson.AppendLine("\"Name\"" + " : \"" + aSceneName + "\",");
    }

    public static void EndScene()
    {
        myCurrentExportJson.AppendLine("},");
    }

    public static void AddToExport<T>(T data, bool endObject = false)
    {
        string jsonData = string.Empty;
        jsonData = JsonUtility.ToJson(data);
        Debug.Log(jsonData);
        int removeFirst = jsonData.IndexOf('{');
        jsonData = jsonData.Remove(removeFirst, 1);

        int removeLast = jsonData.LastIndexOf('}');
        jsonData = jsonData.Remove(removeLast, 1);

        if(jsonData[jsonData.Length - 1] == ',')
        {
            jsonData.Remove(jsonData.Length - 1, 1);
        }
        myCurrentExportJson.Append(jsonData);
        if(!endObject)
            myCurrentExportJson.Append(",");      
    }

    public static void Save(string name, StringBuilder aBuilder)
    {
        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\";
        if (!System.IO.Directory.Exists(savePath))
            System.IO.Directory.CreateDirectory(savePath);

        System.IO.File.WriteAllText(savePath + name + ".json", aBuilder.ToString());
    }

    public static void EndExport(string aFolder, string aJsonFileName)
    {
        myCurrentExportJson.Remove(myCurrentExportJson.Length - 3, 1);/// = ' ';
        myCurrentExportJson.Append("\n]");
        myCurrentExportJson.Append("\n}");


        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\";
        if (!System.IO.Directory.Exists(savePath))
            System.IO.Directory.CreateDirectory(savePath);

        //savePath += aSceneName + ".json";
        savePath += aJsonFileName + ".json";
        System.IO.File.WriteAllText(savePath , myCurrentExportJson.ToString());
        myCurrentExportJson.Clear();

        AssetDatabase.Refresh();
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
