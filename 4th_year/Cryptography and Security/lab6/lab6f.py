from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes

key = b'O cheie oarecare'
data = b'testtesttesttesttesttesttesttesttesttesttest'

# Creare cipher cu GCM, putem face criptare pe orice lungime de date
cipher = AES.new(key, AES.MODE_GCM)

# Criptare
ciphertext, tag = cipher.encrypt_and_digest(data)

print(f"Date originale: {data}")
print(f"Nonce: {cipher.nonce.hex()}")
print(f"Ciphertext: {ciphertext.hex()}")
print(f"Tag: {tag.hex()}")

# Decriptare
cipher_decrypt = AES.new(key, AES.MODE_GCM, nonce=cipher.nonce)
plaintext = cipher_decrypt.decrypt_and_verify(ciphertext, tag)
print(f"Date decriptate: {plaintext}")

# ## Ce face diferit GCM:

# 1. **Nonce** = un numar aleator folosit o singura data
#    - Face ca acelasi text sa dea ciphertext diferit de fiecare data

# 2. **Tag** = semnatura de autentificare
#    - Detecteaza daca cineva a modificat datele criptate

# 3. **Nu necesita padding** 
#    - Functioneaza cu orice lungime de date

# 4. **Sigur**
#    - Blocuri identice → ciphertext diferit
#    - Pattern-urile sunt ascunse

# ## Comparatie vizuala:

# **ECB (PROST):**
# ```
# Date: AAAA AAAA BBBB
# Cript: X123 X123 Y456  ← se repeta X123!
# ```

# **GCM (BUN):**
# ```
# Date: AAAA AAAA BBBB
# Cript: X123 W789 Z234  ← tot diferit!

# Problema cu ECB:

# Blocuri identice → ciphertext identic 
# Nu foloseste IV (Vector de Initializare)
# Se vad pattern-urile in date