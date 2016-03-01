Title: 버그 삽질기 - 파이썬으로 구현한 API 서버 루비로 포팅하기
Subtitle: HTTP 요청 parameters를 이용하다 밟을수 있는 함정
Tags: python, ruby, bug
Date: 2016-03-01
Author: if1live
Slug: http-request-params-trap-python-and-ruby

간단한 명언 API 서버를 만들보자. 필요한 기능은 다음과 같다.

1. 명언 목록을 데이터로 분리한다. 나중에 목록을 쉽게 갱신할수 있어야한다.
2. 서버에 접속하면 명언중 하나를 임의로 골라서 보여준다.
3. Query String을 이용해서 특정 명언을 골라서 볼수 있어야 한다. (결정론적으로 작동해야 테스트하기 쉬우니까)
4. Query String이 올바르지 않으면 임의로 하나를 보여준다.

다은은 간단한 작성된 명언 목록이다. 한줄에 하나의 명언을 적었다.

```txt
가정이 무너지고 사회가 무너지고
안 되겠소 쏩시다
트위터는 인생의 낭비
비둘기야 먹자
애니메이션 캐릭터에 꼴린다면 정신병원에 가보세요
인간의 욕심은 끝이 없고 같은 실수를 반복한다
```

마이크로 프레임워크 중 하나인 [flask](http://flask.pocoo.org/)로 간단한 API 서버를 만들었다.
GET 요청에 딸려 들어온 query string에서 idx를 찾은 다음에 int로 캐스팅하고 이를 명언 목록의 인덱스로 쓴다. 만약 idx가 없거나 int로 캐스팅하는게 불가능하면 예외가 발생해서 랜덤 명언으로 동작한다.

```python
#!/usr/bin/env python

import flask as fl
import random

messages = open("db.txt").read().splitlines()
msg_count = len(messages)

app = fl.Flask(__name__)

@app.route("/")
def wise_saying():
    try:
        idx = int(fl.request.args["idx"]) % msg_count
    except:
        idx = int(random.random() * msg_count)
    return messages[idx]

if __name__ == "__main__":
    app.run(debug=True, port=4567)
```

서버를 띄운다음에 curl을 이용해서 접속해보자. 명언을 랜덤으로 골라서 적절히 보여준다.
```
$ curl http://127.0.0.1:4567
인간의 욕심은 끝이 없고 같은 실수를 반복한다
$ curl http://127.0.0.1:4567
트위터는 인생의 낭비
```

API서버가 진짜로 랜덤한 명언을 내려주는지 [블랙박스 테스트](//en.wikipedia.org/wiki/Black-box_testing) 해보고 싶다.
서버에 여러번 요청해보고 여러가지 출력이 나온다면 랜덤으로 작동하는거라고 믿을수 있을거다.
이를 테스트하는 스크립트를 작성했다.
지정된 query string으로 서버에 10번의 요청을 날리고 몇가지 출력이 나왔는지 세는 스크립트이다.

```sh
#!/bin/bash

# Usage : ./<script> <example_filename>

function kill_wise_saying_server() {
	rb_pids=$(ps -o pid,command|grep -E "^\s*\d+ .*ruby ./wise_saying_.+\.rb$"|awk '{print $1}')
	py_pids=$(ps -o pid,command|grep -E "^\s*\d+ .*python ./wise_saying_.+\.py$"|awk '{print $1}')

	for pid in ${rb_pids[@]}; do
		kill $pid
		wait $pid 2> /dev/null
	done
	for pid in ${py_pids[@]}; do
		kill $pid
		wait $pid 2> /dev/null
	done
}

SERVER_FILE=$1
QUERY_STRING=$2
TMP_FILE=./log_$SERVER_FILE.log

kill_wise_saying_server
rm -rf $TMP_FILE

./$SERVER_FILE 2> /dev/null &
PID=$!

# wait until server run
for i in {1..60}; do
	output=$(curl http://127.0.0.1:4567/ -s)
	if [[ $output == "" ]]; then
		#echo "server is launching... $i"
		sleep 1
	else
		break
	fi
done

for i in {1..10}; do
	curl http://127.0.0.1:4567/?$QUERY_STRING -s >> $TMP_FILE
	echo "" >> $TMP_FILE
done
uniq_count=$(cat $TMP_FILE | sort | uniq | wc -l | xargs)
echo "query_string : $QUERY_STRING => $uniq_count"

kill $PID
wait $PID 2> /dev/null

kill_wise_saying_server
```

Query string 없이 요청을 날려봤다. 여러가지 출력이 나왔다.
랜덤 명언 API 서버가 잘 작동하나보다.

```
$ ./test_server.sh wise_saying_flask.py
query_string :  => 6
```

명언 인덱스를 명시하고 요청을 날렸다. 출력은 한가지로 고정된다.
요구사항에 맞는 동작이다.

```
$ ./test_server.sh wise_saying_flask.py idx=1
query_string : idx=1 => 1
```

올바르지 않은 인자를 서버에 넘겼다. 서버는 이를 int로 파싱하지 못하기때문에 랜덤 명언 API로 작동한다.
요구사항에 맞는 동작이다.

```
$ ./test_server.sh wise_saying_flask.py idx=asdf
query_string : idx=asdf => 5
```

## Sinatra version

[Sinatra](http://www.sinatrarb.com/)는 루비기반의 유사한 마이크로 프레임워크이다.
flask와 같은 구조를 유지하면서 sinatra로 포팅해봤다.
(또는 "파이썬에서는 이렇게 작동했으니 루비에서도 그렇게 작동하겠지"라고 기대하고 코딩을 하거나)

```ruby
#!/usr/bin/env ruby

require "sinatra"

messages = File.read("db.txt").split("\n")
msg_count = messages.length

get '/' do
  begin
    idx = params["idx"].to_i % msg_count
  rescue
    idx = (Random.rand * msg_count).to_i
  end
  messages[idx]
end
```

flask 기반의 서버를 블랙 박스 테스트 한것과 똑같은 방식으로 테스트를 해보자.

query string 없이 요청을 날려보았다.
랜덤이 아니다? 한가지 출력만 나온다?

```
$ ./test_server.sh wise_saying_sinatra.rb
query_string :  => 1
```

명언 인덱스를 명시하고 요청을 날렸다. 출력은 한가지로 고정된다.
요구사항에 맞는 동작이다.

```
$ ./test_server.sh wise_saying_sinatra.rb idx=1
query_string : idx=1 => 1
```

올바르지 않은 명언 인덱스를 붙여서 요청을 날려보았다.
랜덤이 아니다? 한가지 출력만 나온다?

```
$ ./test_server.sh wise_saying_sinatra.rb idx=asdf
query_string : idx=asdf => 1
```

파이썬과 똑같은 방식으로 구현했지만 행동은 같지 않다.
원인은 간단하다. 파이썬과 루비의 행동이 다르기 때문이다.

1. 파이썬에서는 없는 키로 접근하면 예외가 발생하지만 루비는 nil을 얻는다.
2. 파이썬에서는 올바르지 않은 값을 int로 변환하면 예외가 발생하지만 루비는 0이 된다.

그래서 sinatra로 구현한 랜덤 명언 API 서버는 제대로 동작하지 않는다.

## Sinatra version (bug fixed)

파이썬에서는 파이썬의 방식, 루비에서는 루비의 방식으로 다시 구현하는 방법도 있지만 새로 작성하는 일은 귀찮다.
코드의 구조는 그대로 유지하면서 루비에서도 제대로 작동하게 만들고 싶으면 다음을 이용하면 된다.

* `#to_i` -> `Kernel#Integer`
* `[]` -> `#fetch`

```ruby
#!/usr/bin/env ruby

require "sinatra"

messages = File.read("db.txt").split("\n")
msg_count = messages.length

get '/' do
  begin
    idx = Integer(params.fetch("idx")) % msg_count
  rescue
    idx = (Random.rand * msg_count).to_i
  end
  messages[idx]
end
```

```
$ ./test_server.sh wise_saying_sinatra_alt.rb
query_string :  => 5
```

```
$ ./test_server.sh wise_saying_sinatra_alt.rb idx=1
query_string : idx=1 => 1
```

```
$ ./test_server.sh wise_saying_sinatra_alt.rb idx=asdf
query_string : idx=asdf => 6
```

## Summary

위의 코드는 실화를 기반으로 작성되었습니다.
같은 사고를 치지 않으려면 언어별 행동 차이를 기억하면서 코드를 작성해야 합니다.
<s>아니면 하나의 프로젝트 안에서는 하나의 코드만 쓰거나</s>

* [올바르지 않은 값을 정수로 변환하기 - 파이썬 vs 루비]({filename}make-int-from-invalid-value-python-and-ruby.md)
* [존재하지 않는 키로 테이블에 접근하면?]({filename}if-key-does-not-exist-then.md)
