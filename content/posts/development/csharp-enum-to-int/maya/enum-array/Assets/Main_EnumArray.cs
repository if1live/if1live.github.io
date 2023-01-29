using System;
using System.Linq.Expressions;
using UnityEngine;

class EnumArray<TEnum, TValue> {
    TValue[] data;

    public EnumArray(int count) {
        data = new TValue[count];
    }

    public TValue this[TEnum key]
    {
        get { return data[ConvertToIndex(key)]; }
        set { data[ConvertToIndex(key)] = value; }
    }

    int ConvertToIndex(TEnum key) {
        return CastTo<int>.From(key);
    }
}

// https://stackoverflow.com/questions/1189144/c-sharp-non-boxing-conversion-of-generic-enum-to-int
/// <summary>
/// Class to cast to type <see cref="T"/>
/// </summary>
/// <typeparam name="T">Target type</typeparam>
public static class CastTo<T> {
    /// <summary>
    /// Casts <see cref="S"/> to <see cref="T"/>.
    /// This does not cause boxing for value types.
    /// Useful in generic methods.
    /// </summary>
    /// <typeparam name="S">Source type to cast from. Usually a generic type.</typeparam>
    public static T From<S>(S s) {
        return Cache<S>.caster(s);
    }

    private static class Cache<S> {
        public static readonly Func<S, T> caster = Get();

        private static Func<S, T> Get() {
            var p = Expression.Parameter(typeof(S), "s");
            var c = Expression.ConvertChecked(p, typeof(T));
            return Expression.Lambda<Func<S, T>>(c, p).Compile();
        }
    }
}


class Main_EnumArray : MonoBehaviour {
    EnumArray<States, IState> states;
    void Start() {
        states = new EnumArray<States, IState>(2);
        states[States.Wait] = new State_Wait();
        states[States.Run] = new State_Run();
    }

    private void Update() {
        var msg = states[States.Run].GetMessage();
    }
}
