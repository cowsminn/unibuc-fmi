1.
transformam in bytes + XOR
One Time Pad este un sistem de criptare perfect sigur daca este folosit corect.

2.
da, cat timp cheia are lungimea egala cu textul criptat si textul clar
key = text_criptat xor text_clar

a trebuit sa adaug un spatiu la 
Orice text clar poate obtinut dintr-un text criptat cu OTP dar cu alta cheie..

ecad8de748ef0b1a857f032101bdb51f5e07c3c37931c37b3c3219ef748215708cf046a18588c1e2f897ca0076ca7f924eb1e6efcb1b905afed5d110228d24049b8242bec6e11d82699409fa1281d7

3.
de evitat, fiecare text trb criptat cu o cheie unica
k - cheia comuna
a, b - mesaje clare
a_c, b_c - mesaje criptate
a_c xor b_c = (a xor k) xor (b xor k) = a xor b

3.2
fiind criptate cu o cheie comuna putem scrie:
m1 xor m2 = 0x07 xor 0x41 = 0x46 = 'F'
daca luam m1 = spatiu = 0x20 atunci =>
0x20 xor 0x46 = 0x66 = 'f' = m2 (si invers)

3.3
m1 xor m2 = 0x66 xor 0x32 = 0x54 
m1 xor m3 = 0x66 xor 0x23 = 0x45
m2 xor m3 = 0x32 xor 0x23 = 0x11

m1 = 0x20 => m2 = 0x20 xor 0x54 = 0x74 = 't' si m3 = 0x20 xor 0x45 = 0x65 = 'e'
(m1,m2,m3) = (' ', 't', 'e')

m2 = 0x20 => m1 = 0x20 0x54 = 0x74 = 't' si m3 = 0x20 xor 0x11 = 0x31 = '1'
(m1,m2,m3) = ('t', ' ', '1')

m3 = 0x20 => m1 = 0x20 0x45 = 0x65 = 'e' si m2 = 0x20 xor 0x11 = 0x31 = '1'
(m1,m2,m3) = ('e', '1', ' ')

4.
 1. 2^256
 2. 2^30 per secunda => 2^256/2^30 = 2^226 secunde 
 3. nu este eficient, timpul este foarte foarte foarte mare
