using System.Collections.Generic;
using UnityEngine;

public class Main_Manual : MonoBehaviour {
    Dictionary<States, IState> dict;

    class StatesComparer : IEqualityComparer<States> {
        public bool Equals(States x, States y) {
            return x == y;
        }
        public int GetHashCode(States obj) {
            return (int)obj;
        }
    }

    void Start() {
        dict = new Dictionary<States, IState>(new StatesComparer())
        {
            { States.Run, new State_Run() },
            { States.Wait, new State_Wait() },
        };
    }

    void Update() {
        var a = dict.ContainsKey(States.Run);
        var b = dict[States.Wait];
    }
}
