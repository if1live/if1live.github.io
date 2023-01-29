interface IState {
    string GetMessage();
}

class State_Wait : IState {
    public string GetMessage() {
        return "wait";
    }
}

class State_Run : IState {
    public string GetMessage() {
        return "run";
    }
}

enum States {
    Wait,
    Run,
}