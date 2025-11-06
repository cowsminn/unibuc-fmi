# from  Crypto.Cipher import AES
# #putem cripta doar bytes de aceea punem b, nu putem cripta stringuri
# key = b'O cheie oarecare'
# data = b'testtesttesttesttesttesttesttesttesttesttesttest'

# cipher = AES.new(key, AES.MODE_ECB)
# ct = cipher.encrypt(data)
# print(ct) # sau print(ct.hex())

from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

key = b'O cheie oarecare'
data = b'test'

cipher = AES.new(key, AES.MODE_ECB)
padded_data = pad(data, AES.block_size)
ciphertext = cipher.encrypt(padded_data)

print(f"Date originale: {data}")
print(f"Date cu padding: {padded_data}")
print(f"Date criptate: {ciphertext}")

# 1 - datele criptate
# 2 - modul de operare ECB(electronic codebook), deterministic (genereaza mereu acelasi lucru din aceeasi cheie si date)
# 3 - nu este sigur pentru date mari sau structurate, deoarece aceleasi bloc, paternuri de date, nu foloseste vector de initializare
    # - dar poate fi folosit pentru date mici (<16 bytes)
# 4 - 16 bytes (128 de biti)