class Program {
    static readonly int A = 100;
    static readonly float B = 0.7f;

    static int a() {
        int x = (int)((float)A * B);
        return x;
    }
    static int b() {
        float c = (float)A * B;
        int x = (int)c;
        return x;
    }

    static void Main(string[] args) {
        System.Console.WriteLine(a());
        System.Console.WriteLine(b());
    }
}

// Visual Studio 2017, .Net Framework 4.6.1
// Debug
// 69
// 70
// Debug
// 69
// 69
