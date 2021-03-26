using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public static class BinaryWriterExtensions
{
    /// <summary>  
    /// <list type="bullet">Max: 16 Chars</list>
    /// </summary>
    public static void Write(this BinaryWriter aBinWriter, string aData)
    {
        int size = Mathf.Min(16, aData.ToCharArray().Length);
        aBinWriter.Write(size);
        aBinWriter.Write(aData.ToCharArray(0, size));
    }

    /// <summary>  
    /// <list type="bullet">Writes Max: 16 Chars of Each String found in the List</list>
    /// </summary>
    public static void Write(this BinaryWriter aBinWriter, List<string> someData)
    {
        foreach (var text in someData)
            Write(aBinWriter, text);
    }

    /// <summary>  
    /// <list type="bullet">X</list>
    /// <list type="bullet">Y</list>
    /// </summary>
    public static void Write(this BinaryWriter aBinWriter, Vector2 aData)
    {
        aBinWriter.Write(aData.x);
        aBinWriter.Write(aData.y);
    }

    /// <summary>  
    /// <list type="bullet">X</list>
    /// <list type="bullet">Y</list>
    /// <list type="bullet">Z</list>
    /// </summary>
    public static void Write(this BinaryWriter aBinWriter, Vector3 aData)
    {
        aBinWriter.Write(aData.x);
        aBinWriter.Write(aData.y);
        aBinWriter.Write(aData.z);
    }

    /// <summary>  
    /// <list type="Bullet">Array Of </list>
    /// <list type="bullet">X</list>
    /// <list type="bullet">Y</list>
    /// <list type="bullet">Z</list>
    /// <list type="bullet">W</list>
    /// </summary>
    public static void Write(this BinaryWriter aBinWriter, Vector3[] someData)
    {
        foreach (Vector3 data in someData)
        {
            aBinWriter.Write(data);
        }
    }

    /// <summary>  
    /// <list type="bullet">Position</list>
    /// <list type="bullet">Rotation Euler Angles</list>
    /// <list type="bullet">Local Scale</list>
    /// </summary>
    public static void Write(this BinaryWriter aBinWriter, Transform aData)
    {
        aBinWriter.Write(aData.position);
        aBinWriter.Write(aData.rotation.eulerAngles);
        aBinWriter.Write(aData.localScale);
    }

    public static void Write(this BinaryWriter aBinWriter, int aID, Mesh aMesh)
    {
        aBinWriter.Write(aID);
        aBinWriter.Write(aMesh.colors.Length);

        Vector3[] colorsRGB = new Vector3[aMesh.colors.Length];
        for (int i = aMesh.colors.Length - 1; i > -1; --i)
        {
            colorsRGB[i].x = aMesh.colors[i].r;
            colorsRGB[i].y = aMesh.colors[i].g;
            colorsRGB[i].z = aMesh.colors[i].b;
        }
        aBinWriter.Write(colorsRGB);

        Vector3[] vertexPositions = aMesh.vertices;
        aBinWriter.Write(vertexPositions.Length);
        aBinWriter.Write(vertexPositions);
    }
}
