1. niciunul dintre candidate nu genereaza o secventa de numere care pare aleatoare.
	- xor (mereu 0)
	- secventa liniara
	- shiftam acelasi numar mereu la dreapta

3.1	- seed ul este constant, atacatorul poate genera aceeasi secventa daca cunoaste seedul
	- functia rand() nu este criptografic sigura, seedul se bazeaza pe userid, deci va genera acelasi SesionID(daca un atacator cunoase userid poate prezice sesionId)

3.2 CWE-330: Use of Insufficiently Random Values si CWE-338: Use of Cryptographically Weak Pseudorandom Number Generator

3.3 un atacator poate face brute force (genereaza seed urile posibile), CWE corespunzător: CWE-331: Insufficient Entropy

3.4 capec 59

3.5 cele de mai sus, 330, 331, 338

3.6 21