class Program {
    static int a() {
        int a = 100;
        float b = 0.7f;
        int x = (int)((float)a * b);
        return x;
    }
    static int b() {
        int a = 100;
        float b = 0.7f;
        float c = (float)a * b;
        int x = (int)c;
        return x;
    }

    static void Main(string[] args) {
        System.Console.WriteLine(a());
        System.Console.WriteLine(b());
    }
}

// Visual Studio 2017, .Net Framework 4.6.1
// 69
// 70
