using UnityEngine;

static class IronWroughtExtensions
{
    public static Vector3 ConvertToIronWroughtRotation(this Transform aTransform)
    {
        Vector3 eulerAngles = aTransform.eulerAngles;
        //Add any changes in Rotation for fixing anything in IronWrought here
        eulerAngles.x = (-eulerAngles.x) - 360.0f;
        eulerAngles.y += 180.0f;
        eulerAngles.z = (-eulerAngles.z) - 360.0f;
        return eulerAngles;
    }

    public static Vector3 ConvertToIronWroughtPosition(this Transform aTransform)
    {
        Vector3 position = aTransform.position;
        //Add any changes in Position for fixing anything in IronWrought here
        return position;
    }

    public static Vector3 ConvertToIronWroughtScale(this Transform aTransform)
    {
        Vector3 scale = aTransform.localScale;
        //Add any changes in Scale for fixing anything in IronWrought here
        return scale;
    }
}


