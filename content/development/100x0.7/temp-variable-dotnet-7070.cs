public class Test {
    static int a() {
        return (int)((float)100 * 0.7f);
    }

    static int b() {
        float c = (float)100 * 0.7f;
        return (int)c;
    }

    public static void Main() {
        System.Console.WriteLine(a());
        System.Console.WriteLine(b());
    }
}

// Visual Studio 2017, .Net Framework 4.6.1
// 70
// 70
