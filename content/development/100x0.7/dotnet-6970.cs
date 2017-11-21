class Program {
    readonly static int A = 100;
    readonly static float B = 0.7f;

    static int Calc_1() {
        int x = (int)((float)A * B);
        return x;
    }
    static int Calc_2() {
        float c = (float)A * B;
        int x = (int)c;
        return x;
    }


    static void Main(string[] args) {
        System.Console.WriteLine(Calc_1());
        System.Console.WriteLine(Calc_2());
    }
}

// Visual Studio 2017, .Net Framework 4.6.1
// 69
// 70
