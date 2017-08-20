using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Main_Simple : MonoBehaviour {
    Dictionary<States, IState> dict;

    void Start () {
        dict = new Dictionary<States, IState>()
        {
            { States.Run, new State_Run() },
            { States.Wait, new State_Wait() },
        };
    }

    void Update () {
        var a = dict.ContainsKey(States.Run);
        var b = dict[States.Wait];
    }
}
