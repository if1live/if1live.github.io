using System;
using UnityEngine;
using Assets.Chiho;
using System.Reflection;

public class Main : MonoBehaviour {
	void Start () {
        var p1 = new ParameterContainer();
        var p2 = p1.Clone() as ParameterContainer;

        try
        {
            p2.Push("foo");
            p2.Push("bar");
            p2.Push("spam");
        }
        catch (StackOverflowException e)
        {
            Debug.LogException(e);
        }

        //var t1 = p1.GetType().BaseType;
        //var t2 = p2.GetType().BaseType;
        var t1 = p1.GetType().GetBaseType();
        var t2 = p2.GetType().GetBaseType();
        Debug.Log(t1.IsAssignableFrom(t2));
    }
}


