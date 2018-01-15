using System;
//#if UNITY_WSA && !UNITY_EDITOR
using System.Reflection;
//#endif

namespace Assets.Chiho
{
    public static class TypeUtils
    {
        public static Type GetBaseType(this Type type)
        {
#if UNITY_WSA && !UNITY_EDITOR
            return type.GetTypeInfo().BaseType;
#else
            return type.BaseType;
#endif
        }
    }
}