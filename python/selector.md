## select module / selectors

```python
sel = selectors.DefaultSelector()

sel.register(fp1, selectors.EVENT_READ)
sel.register(fp2, selectors.EVENT_READ)
sel.register(fp3, selectors.EVENT_READ)

for key, mask in sel.select():
    print(key.fileobj)

```

## uvloop
https://pypi.org/project/uvloop/