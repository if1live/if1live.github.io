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
