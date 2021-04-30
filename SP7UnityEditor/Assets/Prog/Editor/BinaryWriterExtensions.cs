using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class BinaryWriterExtensions
{
    //public static string Write<T>(this BinaryWriter aBinWriter, T someData) where T : struct
    //{
    //    long currentPosition = aBinWriter.BaseStream.Position;
    //    int hashCode = someData.GetType().GetHashCode();
    //    aBinWriter.Write(hashCode);
    //    aBinWriter.Write(someData);
    //    string jsonString = "\"" + someData.GetType().Name + "\" : " + hashCode; 
    //    return "";
    //}


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

    public static void Write(this BinaryWriter aBinWRiter, List<Transform> someData)
    {
        aBinWRiter.Write(someData.Count);
        foreach (Transform data in someData)
            aBinWRiter.Write(data.GetInstanceID());
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

    public static void Write(this BinaryWriter aBinWriter, List<int> someData)
    {
        aBinWriter.Write(someData.Count);
        foreach (var data in someData)
        {
            aBinWriter.Write(data);
        }
    }

    public static void Write(this BinaryWriter aBinWriter, List<TransformLink> someData)
    {
        aBinWriter.Write(someData.Count);
        foreach (var data in someData)
        {
            aBinWriter.Write(data.instanceID);
            aBinWriter.Write(data.position);
            aBinWriter.Write(data.rotation);
            aBinWriter.Write(data.scale);
        }
    }


    public static void Write(this BinaryWriter aBinWriter, List<ModelLink> someData)
    {
        aBinWriter.Write(someData.Count);
        foreach (var data in someData)
        {
            aBinWriter.Write(data.instanceID);
            aBinWriter.Write(data.assetID);
            aBinWriter.Write(data.vertexColorID);
        }
    }

    public static void Write(this BinaryWriter aBinWriter, List<PointLight> someData)
    {
        aBinWriter.Write(someData.Count);
        foreach (var data in someData)
        {
            aBinWriter.Write(data.instanceID);
            aBinWriter.Write(data.range);
            aBinWriter.Write(data.r);
            aBinWriter.Write(data.g);
            aBinWriter.Write(data.b);
            aBinWriter.Write(data.intensity);
        }
    }

    public static void Write(this BinaryWriter aBinWriter, List<ColliderLink> someData)
    {
        aBinWriter.Write(someData.Count);
        foreach (var data in someData)
        {
            aBinWriter.Write(data.instanceID);
            aBinWriter.Write(data.positionOffest);
            aBinWriter.Write(data.colliderType);
            aBinWriter.Write(data.boxSize);
            aBinWriter.Write(data.sphereRadius);
            aBinWriter.Write(data.capsuleHeight);
            aBinWriter.Write(data.capsuleRadius);

            aBinWriter.Write(data.isStatic ? 1 : 0);
            aBinWriter.Write(data.isKinematic ? 1 : 0);
            aBinWriter.Write(data.isTrigger ? 1 : 0);

            aBinWriter.Write(data.mass);
            aBinWriter.Write(data.localMassPosition);
            aBinWriter.Write(data.inertiaTensor);
            aBinWriter.Write(data.dynamicFriction);
            aBinWriter.Write(data.staticFriction);
            aBinWriter.Write(data.bounciness);
        }
    }

    public static void Write(this BinaryWriter aBinWriter, List<Enemy> someData)
    {
        aBinWriter.Write(someData.Count);
        foreach (var data in someData)
        {
            aBinWriter.Write(data.instanceID);
            aBinWriter.Write(data.speed);
            aBinWriter.Write(data.radius);
            aBinWriter.Write(data.health);
            aBinWriter.Write(data.attackDistance);
            //Refactoring Patrol Points out of Enemy into "EnemyPatrolPoints" Not yet implemented with JSon Export /Axel Savage 2021-04-23
        }
    }


    public static void Write(this BinaryWriter aBinWriter, List<EventData> someData)
    {
        aBinWriter.Write(someData.Count);
        foreach (var data in someData)
        {
            aBinWriter.Write(data.instanceID.GetInstanceID());
            aBinWriter.Write(data.gameEvent);
        }
    }

    public static void Write(this BinaryWriter aBinWriter, ParentData data)
    {
        aBinWriter.Write(data.parent.GetInstanceID());
        aBinWriter.Write(data.children);
    }

    public static void Write(this BinaryWriter aBinWRiter, InstancedModel data)
    {
        aBinWRiter.Write(data.assetID);
        aBinWRiter.Write(data.transforms.Count);
        foreach(STransform transformData in data.transforms)
        {
            aBinWRiter.Write(transformData.position);
            aBinWRiter.Write(transformData.rotation);
            aBinWRiter.Write(transformData.scale);
        }
    }

    public static void Write(this BinaryWriter aBinWriter, InstanceIDCollection data)
    {
        aBinWriter.Write(data.Ids);
    }

    public static void Write(this BinaryWriter aBinWriter, TransformCollection data)
    {
        aBinWriter.Write(data.transforms);
    }

    public static void Write(this BinaryWriter aBinWriter, VertexColorCollection data)
    {
        //TODO
        //Move Material Names into Resource_Assets.json
    }

    public static void Write(this BinaryWriter aBinWriter, ModelCollection data)
    {
        aBinWriter.Write(data.models);
    }

    public static void Write(this BinaryWriter aBinWriter, DirectionalLightCollection data)
    {
        // Plz fix /Nico 28/4

        //aBinWriter.Write(data.directionalLight.instanceID);
        //aBinWriter.Write(data.directionalLight.direction);
        //Vector3 rgb = new Vector3(data.directionalLight.r, data.directionalLight.g, data.directionalLight.b);
        //aBinWriter.Write(rgb);
        //aBinWriter.Write(data.directionalLight.intensity);
        //aBinWriter.Write(0); //TODO Change to AssetID for Textures
        ////Write(aBinWriter, data.directionalLight.cubemapName);
    }

    public static void Write(this BinaryWriter aBinWriter, PointLightCollection data)
    {
        aBinWriter.Write(data.lights);
    }

    public static void Write(this BinaryWriter aBinWriter, DecalCollection data)
    {
        //TODO
        //Move "MaterialName" in Decal Struct into Resource_Assets.json
    }

    public static void Write(this BinaryWriter aBinWriter, Player data)
    {
        aBinWriter.Write(data.player.instanceID);

        aBinWriter.Write(data.player.childrenIDs);
    }

    public static void Write(this BinaryWriter aBinWriter, BluePrintCollection data)
    {
        //For now We're leaving blueprint linker as Json.
        //The amount of data is pretty small anyways
    }

    public static void Write(this BinaryWriter aBinWriter, ColliderCollection data)
    {
        aBinWriter.Write(data.colliders);
    }

    public static void Write(this BinaryWriter aBinWriter, EnemyCollection data)
    {
        //TODO Fix Enemy Patrol Positions with JSON Exporter!
        //aBinWriter.Write(data.enemies);
    }

    public static void Write(this BinaryWriter aBinWriter, ParentsCollection data)
    {
        aBinWriter.Write(data.parents.Count);
        foreach(ParentData parentData in data.parents)
        {
            aBinWriter.Write(parentData);
        }
    }

    public static void Write(this BinaryWriter aBinWriter, EventCollection data)
    {
        aBinWriter.Write(data.triggerEvents);
    }

    public static void Write(this BinaryWriter aBinWriter, InstanceModelCollection data)
    {
        aBinWriter.Write(data.instancedModels.Count);
        foreach(InstancedModel instancedModelData in data.instancedModels)
            aBinWriter.Write(instancedModelData);
    }
}
