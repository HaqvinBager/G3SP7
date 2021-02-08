using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public static class EditorExtensions 
{
    public static void Ping(this Object aObject)
    {
        EditorGUIUtility.PingObject(aObject);
    }
}
