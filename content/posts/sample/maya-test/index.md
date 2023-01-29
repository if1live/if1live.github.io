---
title: Maya Test Page
subtitle: Simple Maya Test
tags: [maya, sample-article]
slug: maya-test
author: if1live
date: 2010-01-03
url: /posts/maya-test
---

[maya](https://github.com/if1live/maya) is markdown preprocessor for static site generator.

## view

embed file.

```python
import json
x = json.dumps({
    'name': 'maya',
    'repo': 'github.com/if1live/maya',
}, indent=2)
print(x)
```

embed with line range.

```python
x = json.dumps({
    'name': 'maya',
    'repo': 'github.com/if1live/maya',
}, indent=2)
```

## execute

```bash
$ python sample.py
{
  "repo": "github.com/if1live/maya", 
  "name": "maya"
}
```

## gist

{{< gist if1live 000ad3a34cb25c3386dd >}}

## youtube

{{< youtube awfptUKbWEo >}}

