public class Hello1
{
    const int WIN_MODE_FULLSCREEN = 1 << 0;
    const int WIN_MODE_STENCIL = 1 << 1;
    const int WIN_MODE_DOUBLE_BUFFER = 1 << 2;

    public static int create_window(int mode)
    {
        if(mode & WIN_MODE_FULLSCREEN == WIN_MODE_FULLSCREEN) {
            System.Console.WriteLine("fullscreen");
        }
        if(mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL) {
            System.Console.WriteLine("stencil");
        }
        if(mode & WIN_MODE_DOUBLE_BUFFER == WIN_MODE_DOUBLE_BUFFER) {
            System.Console.WriteLine("double buffer");
        }
        return 0;
    }

    public static void Main()
    {
        create_window(WIN_MODE_FULLSCREEN | WIN_MODE_DOUBLE_BUFFER);
    }
}
