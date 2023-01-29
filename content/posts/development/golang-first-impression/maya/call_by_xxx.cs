using System;

public class SampleClass {
    public SampleClass() { this.a = 0; }
    public int a;
}

public struct SampleStruct {
    public SampleStruct(int a) { this.a = a; }
    public int a;

}

public class Program
{
    public static void CallByValue(SampleStruct s) {
        s.a += 1;
        Console.WriteLine("in function : {0}", s.a);
    }
    public static void CallByReference(SampleClass s) {
        s.a += 1;
        Console.WriteLine("in function : {0}", s.a);
    }

    public static void Main()
    {
        SampleStruct s2 = new SampleStruct(0);
        Console.WriteLine("before call by value : {0}", s2.a);
        CallByValue(s2);
        Console.WriteLine("after call by value : {0}", s2.a);

        SampleClass s1 = new SampleClass();
        Console.WriteLine("before call by reference : {0}", s1.a);
        CallByReference(s1);
        Console.WriteLine("after call by reference : {0}", s1.a);
    }
}
