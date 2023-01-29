using System;
// for List, Dictionary
using System.Collections.Generic;

public class Simple {
    public Simple(int a)
    {
        this.a = a;
    }
    public int a;
}

public class Program
{
    public static void Main()
    {
        // list
        List<Simple> dummyList = new List<Simple>()
            {
                new Simple(1),
            };
        dummyList.Add(new Simple(2));
        foreach(Simple s in dummyList) {
            Console.WriteLine("{0} = {1}", s, s.a);
        }

        // dictionary
        Dictionary<string, Simple> dummyMap = new Dictionary<string, Simple>()
            {
                { "foo", new Simple(1) },
            };
        dummyMap["bar"] = new Simple(2);
        foreach(KeyValuePair<string, Simple> kv in dummyMap) {
            Console.WriteLine("{0} = {1}", kv.Key, kv.Value);
        }
    }
}
