using UnityEngine;

using System;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading.Tasks;

[DataContract]
internal class SimpleReq
{
    [DataMember]
    public int foo;
}


public class Main : MonoBehaviour
{
    private static readonly HttpClient client = new HttpClient();
    private static readonly string host = "http://127.0.0.1:3100";

    async void Start()
    {
        await RequestCommon(HttpMethod.Post);
        await RequestCommon(HttpMethod.Get);
    }

    public static string ToJsonString<T>(T data)
    {
        var stream1 = new MemoryStream();
        var ser = new DataContractJsonSerializer(typeof(T));
        ser.WriteObject(stream1, data);

        stream1.Position = 0;
        StreamReader sr = new StreamReader(stream1);
        var jsonBody = sr.ReadToEnd();

        return jsonBody;
    }

    static async Task<bool> RequestCommon(HttpMethod method)
    {
        var body = ToJsonString(new SimpleReq() { foo = 1 });
        var request = new HttpRequestMessage(method, $"{host}/")
        {
            Content = new StringContent(body, Encoding.UTF8, "application/json"),
        };
        var resp = await client.SendAsync(request);

        Debug.Log(await resp.Content.ReadAsStringAsync());
        return true;
    }
}
