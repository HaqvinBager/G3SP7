using UnityEngine;
using System.Text;
using UnityEditor;

public static class Json
{
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

    public static bool TryIsValidExport<T>(T obj, out GameObject prefabParent, bool aIsStatic = false) where T : Object
    {
        prefabParent = PrefabUtility.GetOutermostPrefabInstanceRoot(obj);
        
        if (prefabParent == null)
            return false;

        if (prefabParent.isStatic != aIsStatic)
            return false;

        if (PrefabUtility.GetPrefabAssetType(prefabParent) != PrefabAssetType.Regular)
            return false;

        return true;
    }


}
