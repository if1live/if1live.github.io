using System;
using UnityEngine;

class EnumArrayGarbage<TEnum, TValue> {
    TValue[] data;

    public EnumArrayGarbage(int count) {
        data = new TValue[count];
    }

    public TValue this[TEnum key]
    {
        get { return data[ConvertToIndex(key)]; }
        set { data[ConvertToIndex(key)] = value; }
    }

    int ConvertToIndex(TEnum key) {
        var i = Convert.ChangeType(key, typeof(int));
        return (int)i;
    }
}


class Main_EnumArrayGarbage : MonoBehaviour {
    EnumArrayGarbage<States, IState> states;
    void Start() {
        states = new EnumArrayGarbage<States, IState>(2);
        states[States.Wait] = new State_Wait();
        states[States.Run] = new State_Run();
    }

    private void Update() {
        var msg = states[States.Run].GetMessage();
    }
}
