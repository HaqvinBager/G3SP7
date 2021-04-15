using System.Collections;
using System.Collections.Generic;
using UnityEngine;


/*
      "Ids": {
        "byteStart": 0,
        "sizeof" : 4,
        "count": 4     
      },
      "Transforms": {
        "byteStart": 20,
        "sizeof": 28,
        "count": 4
      },
      "Models": {
        "byteStart": 132,
        "sizeof": 12,
        "count" : 1
      },
 */

public enum IronType
{
    ID,
    Transform,
    Model,
    DirectionalLight,
    PointLight,
    Collider,
    InstancedModel
}

public struct Chunk
{
    public IronType aType;
    public int byteStart;
    public int sizeOf;
    public int count;
}

public struct BinStructure
{
    public List<Chunk> chunks;
}

public class ExportBin 
{
    private string myFileName;
    private string myDestination;
    private BinStructure myStructure;

    public ExportBin(string aFileName, string aDestination)
    {
        myFileName = aFileName;
        myDestination = aDestination;
    }

    public void Add(Chunk aChunk)
    {
        myStructure.chunks.Add(aChunk);
    }

    public void Export()
    {

    }
}
