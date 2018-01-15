using System;

#if NETFX_CORE
class StackOverflowException : Exception {
    public StackOverflowException(string message) : base(message) {}
}
#endif

public class ParameterContainer : ICloneable
{
    const int MaxParameterSize = 2;
    string[] parameters = new string[MaxParameterSize];
    int top = 0;

    public ParameterContainer()
    {
#if NETFX_CORE
        var args = new string[0];
#else
        var args = System.Environment.GetCommandLineArgs();
#endif
        for (int i = 0; i < args.Length; i++)
        {
            Push(args[i]);
        }
    }

    public void Push(string arg)
    {
        if (top >= parameters.Length)
        {
            var msg = String.Format("arg : {0}", arg);
            throw new StackOverflowException(msg);
        }

        parameters[top] = arg;
        top++;
    }

    public object Clone()
    {
        return new ParameterContainer()
        {
            top = top,
            parameters = parameters.Clone() as string[],
        };
    }
}
