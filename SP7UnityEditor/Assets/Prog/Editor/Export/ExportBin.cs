//using System;
//using System.Collections;
//using System.Collections.Generic;
//using System.IO;
//using UnityEngine;
//using UnityEngine.SceneManagement;
//using UnityEditor.SceneManagement;


/*
      "Ids": {
       "byteStart": 0, 
        "sizeof" : 4,
        "count": 4     
      }, 
      "Models": {
        "byteStart": 132,
        "sizeof": 12,
        "count" : 1
      },
      "Transforms": {
        "byteStart": 20,
        "sizeof": 28,
        "count": 4
      },
 */

//public enum IronType
//{
//    ID,
//    Transform,
//    Model,
//    DirectionalLight,
//    PointLight,
//    Collider,
//    InstancedModel
//}

//public struct Chunk
//{
//    public IronType ironType;
//    public int byteStart;
//    public int sizeOf;
//    public int count;
//}

//public struct BinStructure
//{
//    public List<Chunk> chunks;
//}

//public class ExportBin 
//{
//    private string myFileName;
//    private string myDestination;
//    private BinStructure myStructure;

//    public ExportBin(string aFileName, string aDestination)
//    {
//        myFileName = aFileName;
//        myDestination = aDestination;
//    }

//    public void DoExport(LevelCollection levels)
//    {
//        BinaryWriter writer = new BinaryWriter(new FileStream(Application.dataPath + "/Generated/TestBinary.bin", FileMode.CreateNew));
//        foreach(SceneReference scene in levels.myScenes)
//        {
//            if (!scene.ScenePath.Contains("Layout"))
//                continue;

//            Scene activeScene = EditorSceneManager.OpenScene(scene.ScenePath, OpenSceneMode.Single);

//            Transform[] transforms = GameObject.FindObjectsOfType<Transform>();
//            writer.Write(transforms.Length);
//            foreach(Transform transform in transforms)
//            {
//                writer.Write(transform.GetInstanceID());
//            }

//            writer.Write(transforms.Length);
//            foreach(Transform transform in transforms)
//            {
//                writer.Write(transform.ConvertToIronWroughtPosition());
//                writer.Write(transform.ConvertToIronWroughtRotation());
//                writer.Write(transform.ConvertToIronWroughtScale());
//            }
//        }

//        writer.Close();
//    }

//    public void Add(Chunk aChunk)
//    {
//        double beforeNumber = 2.0;
//        Debug.Log(beforeNumber);
//        byte[] bytes = BitConverter.GetBytes(beforeNumber);
//        //BitConverter.GetBytes(test.y);
//        //BitConverter.GetBytes(test.z);
//        double backToNumber = BitConverter.ToDouble(bytes, 0);

//        Debug.Log(backToNumber);
//        myStructure.chunks = new List<Chunk>();
//        myStructure.chunks.Add(aChunk);
//    }

//    public void Export()
//    {

//    }
//}
