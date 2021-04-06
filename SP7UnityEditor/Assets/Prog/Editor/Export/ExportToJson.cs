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
    //static string myFolderName = "";
    static StringBuilder myCurrentExportJson = new StringBuilder();

    public static void BeginExport(string baseSceneName)
    {
        myCurrentExportJson.Clear();
        myCurrentExportJson.AppendLine("{");
        myCurrentExportJson.AppendLine("\"Root\" : \"" + baseSceneName + "\",");
        myCurrentExportJson.Append("\"Scenes\" : [\n");
        mySceneName = baseSceneName;
    }

    public static void BeginScene(string aSceneName)
    {
        myCurrentExportJson.AppendLine("{");
    }

    public static void EndScene()
    {
        myCurrentExportJson.AppendLine("},");
    }

    public static void AddToExport<T>(T data, bool endObject = false)
    {
    
        string jsonData = string.Empty;
        jsonData = JsonUtility.ToJson(data);
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


        string savePath = System.IO.Directory.GetCurrentDirectory() + "\\Assets\\Generated\\" + aFolder + "\\";
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


        if(!objectWithInstanceID.TryGetComponent(out Collider collider))
        {
            if (objectWithInstanceID.isStatic != aIsStatic)
                return false;
        }

        var prefabAssetType = PrefabUtility.GetPrefabAssetType(objectWithInstanceID);
        if (PrefabUtility.GetPrefabAssetType(objectWithInstanceID) != PrefabAssetType.Regular &&
            PrefabUtility.GetPrefabAssetType(objectWithInstanceID) != PrefabAssetType.Variant)
            return false;
        return true;
    }
}


//HOW THIS WORKS
//Using JsonUtility.ToJson will always add 
/*  
 *  how does AddToExport<T>(T data, bool endObject = false) Work? =))
 *  
 * { <-Start Object 
 * 
 * } <- EndObject
 *
 * We want to remove these so that we can Add this object to the main-json we are creating (See Export.cs)
 * Before Remove First and Remove Last have been Applied
 *
 * This is how the string "jsonData" Looks like when we send in a TData struct with two floats (U, V) to JsonUtility.ToJson(<TData>);
 *  { 
 *    "TData" : {
 *          "U" : 0.55
 *          "V" : 0.25
 *    }
 *  } 
 * 
 * The object Above would sadly Not fit inside the greater object as shown below
 * Because we want to add "TData" as a "member" inside the "Level_1_Layout" we are currently saving to.
 * 
 * {
 *  "Scenes" : [
 *      "Level_1_Layout" : {
 *       {                          <-- This And 
 *          "TData" : {
 *              "U" : 0.55,
 *              "V" : 0.25
 *          }
 *       }                          <-- this Causes the break
 *     }
 *  ]
 * }
 * 
 * The Result we want in this case is the following: 
 * {
 *      "Scenes" : 
 *      [
 *          "Level_1_Layout" : {    
 *              "TData" : {   <-- This object now fits inside the "Level_1_Layout" Object (which is conceptually the current object we are writing to)
 *                  "U" : 0.55,
 *                  "V" : 0.25
 *              }
 *          }
 *      ]
 * }
 * 
 * An Example of how this looks like when we add More scenes (eg. Level_1_Layout + Level_1_Lights)
 * {
 *      "Scenes" : 
 *      [
 *          "Level_1_Layout" : {    
 *              "ModelCollection" : [   <-- any random Data Structure For example ModelLink (See ExportModel.cs))
 *                  {
 *                      "InstanceID" : 25153,
 *                      "assetID" : 100   
 *                  },
 *                  {
 *                      "InstanceID" : -55131,
 *                      "assetID" : 101
 *                  }
 *              ]
 *              
 *          },
 *         "Level_1_Lights" : {
 *              "Lights" : [          <-- any random Data Structure For example ModelLink (See ExportPointLights.cs))
 *                  {
 *                      "instanceID": 2551,
 *                      "range" : 100.0,
 *                      "r" : 0.1,
 *                      "g" : 1.0,
 *                      "b" : 0.5
 *                      "intensity" : 10.5
 *                  }
 *              ]
 *         }
 *      ]
 * }
 */
