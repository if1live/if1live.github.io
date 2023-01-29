using UnityEngine;

class Main_SimpleArray : MonoBehaviour {
    IState[] states;
    private void Start() {
        states = new IState[2];
        states[(int)States.Wait] = new State_Wait();
        states[(int)States.Run] = new State_Run();
    }

    private void Update() {
        var msg = states[(int)States.Run].GetMessage();
    }
}

