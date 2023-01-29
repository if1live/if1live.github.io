using System;

namespace Main
{
    public class Program
    {
        public void Run()
        {
            for(int i = 0 ; i < 1; i++)
            {
                Console.WriteLine("indent - c#");
            }
        }
        public static void Main()
        {
            var prog = new Program();
            prog.Run();
        }
    }
}
