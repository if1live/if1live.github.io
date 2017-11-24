using System;

internal class Program
{
	private static readonly int A = 100;

	private static readonly float B = 0.7f;

	private static int a()
	{
		return (int)((float)Program.A * Program.B);
	}

	private static int b()
	{
		return (int)((float)Program.A * Program.B);
	}

	private static void Main(string[] args)
	{
		Console.WriteLine(Program.a());
		Console.WriteLine(Program.b());
	}
}
