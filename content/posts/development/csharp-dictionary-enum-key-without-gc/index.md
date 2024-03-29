---
title: GC없이 C# Dictionary에서 enum을 key로 쓰기
tags: [csharp, unity, c#]
slug: csharp-dictionary-enum-key-without-gc
author: if1live
date: 2017-08-20
url: /posts/csharp-dictionary-enum-key-without-gc
---

## C# Dictionary + enum

C# Dictionary는 Key-Value로 데이터를 저장할수 있는 좋은 자료구조이다.
enum은 key로 쓰기에 좋은 타입이다.
그렇다면 둘을 합쳐보자.

![ppap](ppap.jpg)

적당한 클래스와 enum을 준비한다.

```csharp
﻿interface IState {
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
```

Dictionary을 만들고 데이터를 저장해두자.
`Update()`에서 Dictionary에 접근해보자.
Dictionary를 쓰면 자주 사용할 기능 두 개를 사용했다.
키가 존재하는지 확인하는 함수와 키로 값을 얻는 기능을 사용했다.

```csharp
﻿using System.Collections;
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
```

유니티 프로파일러 띄워서 성능 이슈가 있는지 살펴보자.

![profiler](profiler.png)

`Main_Simple`의 `ContainsKey`, `get_item`에서 GC Alloc이 발생하는 것을 볼수있다. Dictionary에 키가 있는지 확인하고 값을 읽는 것만으로도 GC가 발생하는건 쓸데없는 부하같다. 

이를 최적화해서 GC Alloc이 없는 `Main_Manual`, `Main_Generic`로 만들어보자.

## Custom EnumComparer

프로파일러에서 GC가 생기는 지점을 보면 `DefaultComparer`에서 GC Alloc가 발생하는 것을 알수있다. 
`DefaultComparer` 내부의 boxing 때문에 GC가 발생하는 것일테니 boxing이 없는 Comparer를 구현하면 GC Alloc를 없앨 수 있을 것이다.
`IEqualityComparer`를 구현해서 EnumComparer를 직접 만들어보자. 이를 Dictionary의 생성자로 넣어준다.

```csharp
﻿using System.Collections.Generic;
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
```

##  Generic EnumComparer

코드에 존재하는 enum이 한두개도 아닌데 enum마다 enum comparer를 구현하는건 역시 귀찮다.
제네릭으로 적절한 enum comparer를 찍어낼수 있으면 편할 것이다.
이런 좋은 기능은 남들이 다 구현해뒀으니 갖다쓰면 된다.

[Accelerating Enum-Based Dictionaries with Generic EnumComparer][codeproject-detail]

[Repository][repo]

```csharp
﻿using SitraUtils;
using System.Collections.Generic;
using UnityEngine;

public class Main_Generic : MonoBehaviour {
    Dictionary<States, IState> dict;

    void Start() {
        dict = new Dictionary<States, IState>(EnumComparer.For<States>())
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
```

## IL2CPP

유니티에서 모바일 개발을 하면 IL2CPP는 피할수 없다. 
iOS 11 부터는 64비트 어플리케이션만 굴러간다. 
그리고 유니티에서 64비트 지원을 할수 있는 방법은 아직까지는 IL2CPP뿐이다.

IL2CPP 옵션을 켜고 빌드한후 실행하면 에러가 난다.

```txt
NotSupportedException: C:\Program Files\Unity-2017.1.0f3\Editor\Data\il2cpp\libil2cpp\icalls\mscorlib\System.Reflection.Emit\DynamicMethod.cpp(19) : Unsupported internal call for IL2CPP:DynamicMethod::create_dynamic_method - System.Reflection.Emit is not supported.: so.libsora.dictionaryenumkey
  at System.Reflection.Emit.DynamicMethod.CreateDynMethod () [0x00000] in <filename unknown>:0 : so.libsora.dictionaryenumkey
  at System.Reflection.Emit.DynamicMethod.CreateDelegate (System.Type delegateType, System.Object target) [0x00000] in <filename unknown>:0 : so.libsora.dictionaryenumkey
  at System.Linq.Expressions.Expression`1[TDelegate].Compile () [0x00000] in <filename unknown>:0 : so.libsora.dictionaryenumkey
  at SitraUtils.EnumComparer`1[TEnum]..cctor () [0x00000] in <filename unknown>:0 : so.libsora.dictionaryenumkey
  at SitraUtils.EnumComparer.For[TEnum] () [0x00000] in <filename unknown>:0 : so.libsora.dictionaryenumkey
  at Main_Generic.Start () [0x00000] in <filename unknown>:0 : so.libsora.dictionaryenumkey
Rethrow as TypeInitializationException: The type initializer for 'SitraUtils.EnumComparer<States>' threw an exception.: so.libsora.dictionaryenumkey
  at SitraUtils.EnumComparer.For[TEnum] () [0x00000] in <filename unknown>:0 : so.libsora.dictionaryenumkey
```

IL2CPP에서는 dynamic method를 사용할 수 없다.
그래서 Generic EnumComparer는 쓸수 없다.
IL2CPP를 사용할 경우 수동으로 EnumComparer를 구현해야 된다.

관련 내용은 유니티 문서 [Scripting restrictions][unity-scripting-restrictions]에서 찾을 수 있다.
iOS IL2CPP, android IL2CPP는 `Ahead-of-time compile`의 제약에 걸려있다.
AOT의 제약에서 다음을 찾을수 있다.

> System.Reflection.Emit
> 
> An AOT platform cannot implement any of the methods in the System.Reflection.Emit namespace.
> Note that the rest of System.Reflection is acceptable,
> as long as the compiler can infer that the code used via reflection needs to exist at runtime.


[repo]: https://github.com/OmerMor/EnumComparer

[codeproject-detail]: https://www.codeproject.com/Articles/33528/Accelerating-Enum-Based-Dictionaries-with-Generic

[ios-no-code-gen]: http://ronniej.sfuh.tk/%EC%9C%A0%EB%8B%88%ED%8B%B0-ios-%EC%A0%9C%EC%95%BD%EC%82%AC%ED%95%AD/

[unity-scripting-restrictions]: [https://docs.unity3d.com/Manual/ScriptingRestrictions.html]
