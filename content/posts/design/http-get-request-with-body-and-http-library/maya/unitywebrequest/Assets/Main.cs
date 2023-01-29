using System.Collections;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.IO;
using System.Text;
using UnityEngine;
using UnityEngine.Networking;

[DataContract]
internal class SimpleReq
{
    [DataMember]
    public int foo;
}


public class Main : MonoBehaviour
{
    private static readonly string host = "http://192.168.200.20:3100";

    void Start()
    {
        StartCoroutine(BeginStart());
    }

    IEnumerator BeginStart()
    {
        yield return RequestCommon(UnityWebRequest.kHttpVerbPOST);
        yield return RequestCommon(UnityWebRequest.kHttpVerbGET);
    }


    IEnumerator RequestCommon(string method)
    {
        var body = ToJsonBinary(new SimpleReq() { foo = 1 });
        var www = new UnityWebRequest(host);
        www.method = method;
        www.uploadHandler = new UploadHandlerRaw(body);
        www.uploadHandler.contentType = "application/json";
        www.downloadHandler = new DownloadHandlerBuffer();
        yield return www.SendWebRequest();

        Debug.Log(www.downloadHandler.text);
    }

    public static byte[] ToJsonBinary<T>(T data)
    {
        var stream1 = new MemoryStream();
        var ser = new DataContractJsonSerializer(typeof(T));
        ser.WriteObject(stream1, data);

        stream1.Position = 0;
        StreamReader sr = new StreamReader(stream1);
        var jsonBody = sr.ReadToEnd();

        byte[] byteArray = Encoding.UTF8.GetBytes(jsonBody);
        return byteArray;
    }
}
