Lab2    

1.
A - 4  
B - 2  
C - 5  
D - 1   
E - 6   
F - 3   

2.
1 - confidentialitate  
2 - disponibilitate  
3 - integritate  
4 - confidentialitate  
5 - integritate  

3.1
Cifrul lui Cezar  
Ne deplasam cu 3 pozitii  
A -> D  
B -> E  
C -> F  

Exemplu criptare (deplasam literele 3 pozitii)  
C - F  
R - U  
I - L  
P - S  
T - W  
O - R  

Exemplu decriptare (deplasam literele -3 pozitii)  
F - C  
U - R  
L - I  
S - P  
W - T  
R - O 

Securitatea sistemului este una foarte scazuta, vulnerabil la atac prin forta bruta (testarea tuturor cheilor), analiza frecventei.

3.2 Columnar Transposition Cipher   
Cheia: ROMA (4 litere -> 4 coloane)  
Mesaj: CRIPTOGRAFIE

Exemplu criptare  

1 2 3 4  
R O M A
- - - - - -
C R I P  
T O G R  
A F I E  

Ordine: 4 - 3 - 2 - 1  
Text criptat: PREIGIROFCTA 

Exemplu decriptare   
Text criptat: PREIGIROFCTA   
Impartim textul in 4 coloane de lungime 3  

1 2 3 4  
R O M A
- - - - - -
C R I P  
T O G R  
A F I E 

Si citim pe randuri: CRIPTOGRAFIE

Este mai rezistent decat Cifrul lui Cezar, dar pastreaza frecventa literelor

5.

8: TSRCX O
9: DECRYPT pozitionez pe A A A, si scriu ce am la 8
10: daca am un text cu o litera din COSMIN pe aceeasi pozitie, enigma nu poate cripta o litera in ea insasi