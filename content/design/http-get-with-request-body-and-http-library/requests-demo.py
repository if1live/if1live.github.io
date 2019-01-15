import requests

uri = 'http://127.0.0.1:3100'

r = requests.post(uri, data = {'foo': 1})
print(r.text)

r = requests.get(uri, data = {'foo': 1})
print(r.text)
