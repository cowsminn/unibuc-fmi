import sys
import urllib.request
from binascii import unhexlify, hexlify

ct_hex = [
    "DBED0CFC52CD75C21A9A6A4AAE5B99",
    "DBFA00E15ECE7CDE159C665FA74593",
    "DBED02E743C77FC31F947157A44799",
    "DCE60DEA58D37FDF1B8F644AA24684",
    "C0E906FB43C87DC11E9C6347A2478D",
    "C0F20DE054C862D411816C51A54886",
    "C0E906FB55CC60D913866C44A2478D"
]

# convert to bytes
cts = [unhexlify(x) for x in ct_hex]
n = len(cts[0])
if not all(len(x) == n for x in cts):
    print("err: lungime diferita")
    sys.exit(1)

words_url = "https://raw.githubusercontent.com/dwyl/english-words/master/words_alpha.txt"
words = None
try:
    data = urllib.request.urlopen(words_url, timeout=30).read().decode("utf-8")
    words = data.splitlines()
except Exception as e:
    print("err: descarcare fisier cuvinte")

words15 = set(w for w in words if len(w) == n and w.isalpha() and w.islower())
print(f"nr cuvinte de {n} litere : {len(words15)}")

def derive_key_from_plaintext(ciphertext, plaintext):
    return bytes([c ^ p for c, p in zip(ciphertext, plaintext.encode('ascii'))])

def decrypt_with_key(key, cts):
    outs = []
    for ct in cts:
        pt_bytes = bytes([c ^ k for c, k in zip(ct, key)])
        try:
            outs.append(pt_bytes.decode('ascii'))
        except:
            outs.append(None)
    return outs

ct0 = cts[0]
checked = 0
matches = []

for w in words15:
    checked += 1
    key = derive_key_from_plaintext(ct0, w)
    outs = decrypt_with_key(key, cts)
    if all(o is not None and o in words15 for o in outs):
        matches.append((key, outs))
        break

if not matches:
    print("no matches")
else:
    for key, outs in matches:
        print("Key:", hexlify(key).upper().decode())
        for i, pt in enumerate(outs, start=1):
            print(f"Text clar {i}: {pt}")
