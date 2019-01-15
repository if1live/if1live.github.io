using System;
using RestSharp;
using System.Threading.Tasks;

// RestSharp -Version 106.6.6

class SimpleReq
{
    public int foo;
}

class Program
{
    private static readonly string host = "http://127.0.0.1:3100";

    static async Task<bool> RequestCommon(Method method)
    {
        var client = new RestClient(host);
        var request = new RestRequest("/", method);
        request.AddJsonBody(new SimpleReq() { foo = 1 });

        var resp = await client.ExecuteTaskAsync(request);
        Console.WriteLine(resp.Content);
        return true;
    }
    static void Main(string[] args)
    {
        var taskGET = RequestCommon(Method.GET);
        taskGET.Wait();

        var taskPOST = RequestCommon(Method.POST);
        taskPOST.Wait();
    }
}
