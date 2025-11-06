#!/usr/bin/env python3
import base64, binascii, sys

# --- intrări (editează după nevoie) ---
cipher_b64 = "o9/khC3Pf3/9CyNCbdzHPy5oorccEawZSFt3mgCicRnihDSM8Obhlp3vviAVuBbiOtCSz6husBWqhfF0Q/8EZ+6iI9KygD3hAfFgnzyv9w=="
# pune aici PLAINTEXT-ul pe care vrei să-l obții (fără diacritice, exact aceeaşi lungime în octeți)
target_plaintext = "Orice text clar poate obtinut dintr-un text criptat cu OTP dar cu alta cheie.. "
# --------------------------------------

ct = base64.b64decode(cipher_b64)
pt_bytes = target_plaintext.encode('utf-8')

if len(ct) != len(pt_bytes):
    print(f"EROARE: lungime ciphertext = {len(ct)} bytes, lungime plaintext = {len(pt_bytes)} bytes")

# calculează cheia: key = ct XOR pt
key_bytes = bytes([c ^ p for c,p in zip(ct, pt_bytes)])
key_hex = binascii.hexlify(key_bytes).decode('ascii')

print("Cheia (hex):")
print(key_hex)

# verificare: decodare înapoi = ct XOR key -> trebuie să fie plaintext-ul țintă
recovered = bytes([c ^ k for c,k in zip(ct, key_bytes)])
try:
    rec_text = recovered.decode('utf-8')
except:
    rec_text = "<not valid UTF-8>"

print("\nVerificare: plaintext recuperat:")
print(rec_text)
