1.
a. script obfuscat care atunci cand este rulat afiseaza un mesaj ascuns ( avem variabile ofuscate $ pt a construi secventa de caractere)

b. prezenta variabilelor $, $.$_, $.__$ 

c. node sample1.js 
Facultatea de Matematica si Informatica
Universitatea din Bucuresti
https://www.youtube.com/watch?v=HIcSWuKMwOw
var ascuns = "Mesaj ascuns: 18367622009998665"

d. javascriptobfuscator (diferite setari)
   console.log("test");

2.
 a. avem in consola mesajele You have been hacked! etc folosind wscript.echo, se creeaza un fmi.txt si se scrie in el Bun venit la acest laborator :), dupa care fisierul ii se pune atributul 2 (adica atribut de sistem si este tratat diferit nu se poate modifica si e hidden in file explorer). Daca acest fisier exista se va afisa Dont worry, ghost do not exist!

 eval - fct care executa un script obfuscat

 b. da pt ca se creeza un fisier fara acordul userului,
    //     var obj = new ActiveXObject("Scripting.FileSystemObject"); (acceseaza resursele sistemului)

c. https://www.beautifyconverter.com/javascript-obfuscator.php


4.
a.
scriptul creeaza un binar numit hello.exe in %temp$\hello.exe, dupa care il apeleaza in mod ascuns (Run(...,0,0)), foloseste ActiveXObject care funtioneaza doar pe Windows
Create Object obtine un obiect shell, adodb.steam scrie date binare pe dis, msxml2.domdocument decodeaza un string base64 in bytes
b.


sed -n 's/.*JSZQ92("\([^"]*\)".*/\1/p' sample4.js > payload.b64

base64 -d payload.b64 > hello.exe

file hello.exe
hexdump -C hello.exe | head
sha256sum hello.exe  
strings hello.exe | head


c. descarca si ruleaza un fisier executabil fara acordul utilizatorului (foloseste si ActiveX, folosit in atacur malware) 
d. 25/62, este incadrat ca malware mai exact troian downloader.
e. dupa obfuscare scriptul este detectat doar de 5 antivirusuri, mult mai putin fata de codul initial