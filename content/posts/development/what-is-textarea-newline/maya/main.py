#!/usr/bin/env python
"""
Usage
./main.py runserver_plus 0.0.0.0:8888
"""

from importd import d
# d is initialized before import csrf_exempt
d(DEBUG=True)

from django.views.decorators.csrf import csrf_exempt

question_html = """<html>
<h1>textarea + newline + {method}</h1>
<form action="." method="{method}">
<pre>raw: {raw_text}</pre>
<textarea name="content" rows="10" cols="20">{text}</textarea>
<br/>
<input type="submit"/>
</form>
</html>"""

def render_question(method):
    text = "".join([
        "only-LF\n",
        "only-CR\r",
        "many-LF\n\n",
        "many-cr\r\r",
        "CRLF\r\n",
        "LFCR\n\r",
    ])
    html = question_html.format(method=method, raw_text=repr(text), text=text)
    print("raw html : %r" % html)
    return d.HttpResponse(html)

def render_answer(params):
    content = params["content"]
    output = "content : %r" % content
    return d.HttpResponse(output)

@d("/get/")
def index_get(request):
    if len(request.GET) == 0:
        return render_question("GET")
    else:
        return render_answer(request.GET)

@d("/post/")
@csrf_exempt
def index_post(request):
    if request.method == "GET":
        return render_question("POST")
    else:
        return render_answer(request.POST)

@d("/")
def index(request):
    html = """<html>
<h1>textarea + newline test</h1>
<ul>
<li><a href="/get/">GET version</a></li>
<li><a href="/post/">POST version</a></li>
</ul>
</html>"""
    return d.HttpResponse(html)

if __name__ == "__main__":
    d.main()
