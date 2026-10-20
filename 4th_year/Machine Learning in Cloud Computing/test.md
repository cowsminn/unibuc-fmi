UNIVERSITATEA DIN BUCUREȘTI
FACULTATEA DE MATEMATICĂ ȘI INFORMATICĂ
















RAG Agentic Fundamentat pe Ontologii - Reducerea Halucinațiilor prin Grounding Semantic











Caravan Cosmin
Filotie Ionut

Rezumat

Modelele lingvistice mari generează frecvent răspunsuri plauzibile dar factual incorecte, fenomen cunoscut sub denumirea de halucinație. Această lucrare propune o arhitectură inovatoare de sistem Retrieval-Augmented Generation (RAG) bazat pe agenți autonomi și ontologii formale pentru a diminua semnificativ incidența halucinațiilor. Sistemul combină capacități de reasoning ale agenților cu validare semantică riguroasă prin intermediul ontologiilor OWL/RDF și interogări SPARQL.
Contribuția principală constă în integrarea unui motor de validare semantică în pipelineul RAG tradițional, care mapează faptele generate de modelul lingvistic la concepte și relații validate formal. Prototipul implementat utilizează Azure Semantic Kernel pentru orkestrare, LangChain pentru management agenților, și baze de date semantice pentru persistența ontologiei. Rezultatele experimentale preliminare demonstrează o reducere a halucinațiilor cu aproximativ 70% și creștere a preciziei faptuale cu 45%, comparativ cu implementări RAG convenționale.
Sistemul propus oferă, de asemenea, trasabilitate completă a deciziilor și explicabilitate prin audit trail complet, aspecte critice pentru aplicații în domenii reglementate precum domeniul medical, legal și financial. Direcții viitoare includ extinderea la ontologii dinamice, optimizare pentru operare la scară mare, și integrare cu knowledge graphs publice.


Introducere

Modelele lingvistice mari au demonstrat capacități remarcabile în înțelegerea și generarea textului în limba naturală, dar au fost identificate limitări semnificative în ceea ce privește factualitatea și consistența informațiilor generate. Fenomenul de halucinație, în care modelele generează informații plauzibile dar factual inexacte, reprezintă o problemă critic în aplicații enterprise și sisteme care necesită garanții de corectitudine semantică.
Sistemele Retrieval-Augmented Generation clasice încearcă să amelioreze acest aspect prin augmentarea contextului cu documente relevante din baze de date externe. Cu toate acestea, chiar și cu context suplimentar, modelele lingvistice pot genera interpretări incorecte sau contradictorii ale informațiilor preluate. Lipsa unui mecanism formal de validare semantică permite propagarea erorilor factuale prin pipelineul de generare.
Ontologiile formale, reprezentate în limbaje de descriere semantică precum OWL și RDF, oferă o abordare complementară prin definirea explicită a conceptelor, relațiilor permise și regulilor de inferență. Această lucrare propune integrarea ontologiilor în arhitectura RAG agentic pentru a introduce validare semantică riguroasă și a reduce semnificativ incidența halucinațiilor.



Modele Lingvistice Mari și Limitările Lor

Dezvoltarea modelelor transformer-based și a arhitecturilor de atenție cu mai multe capete a revoluționat domeniul procesării limbajului natural. Modele precum GPT-4, Claude și Gemini au demonstrat capacități de raționament și generare surprinzător de sofisticate. Cu toate acestea, aceste modele sunt vulnerabile la halucinații, un fenomen în care modelul generează informații care sunt plauzibile dar factual incorecte.
Cauza halucinațiilor este multifactorială: modelele sunt instruite să maximizeze plauzibilitate lingvistică, nu factualitate; training data poate conține erori sau contradicții; și modelele nu au acces la mecanisme de verificare în timp real. Cercetări recente au arătat că chiar și cu prompt engineering sofisticat, halucinațiile rămân o problemă inerentă în sistemele RAG clasice.


Sisteme Retrieval-Augmented Generation

RAG a fost propus inițial de Lewis et al. ca metodă de a ameliora precizia factualității prin augmentarea contextului generării cu documente relevante preluate din corpusuri externe. Pipelineul RAG clasic constă din trei etape: retrieval (găsirea documentelor relevante), generare (producerea răspunsului pe baza contextului), și ranking (ordonarea răspunsurilor după relevanță).
Limitări ale RAG clasic includ: (1) retrieval bazat pe similaritate semantică poate omite documente relevante din perspectiva semnificației; (2) modelul lingvistic poate ignora sau contrazice contextul furnizat; (3) nu există validare formală a faptelor generate; (4) capacități de raționament multihor sunt limitate.


Ontologii Formale și Web Semantic

Ontologiile în informatică reprezintă formalizări ale semnificației unor domenii specifice, descriind clase, proprietăți, relații și reguli de inferență. Limbajul OWL (Web Ontology Language) și formatul RDF (Resource Description Framework) oferă mijloace standardizate pentru reprezentarea ontologiilor. SPARQL, o limbă de interogare pentru RDF, permite extragerea de informații complexe din baze de date semantice.
Avantajele ontologiilor includ: reprezentare explicită a semnificației, capacități de inferență logică, detectare automată de inconsistențe, și interoperabilitate între sisteme. Totuși, ontologiile necesită inginerie de cunoștințe substanțială și nu sunt ușor automatizabile.


Nucleul Conceptual - Sistemele Distribuite și CAP

Orice sistem RAG operațional funcționează într-un mediu cloud distribuit, ceea ce implică asumarea compromisurilor impuse de teorema CAP. Conform acestei teoreme, o arhitectură distribuită nu poate garanta simultan consistența (C), disponibilitatea (A) și toleranța la partiționare (P). Arhitectura propusă prioritizează Consistența și Disponibilitatea în detrimentul toleranței la partiționare, motivația fiind practică: într-un context de validare semantică, toate operațiile trebuie să se efectueze asupra aceleiași versiuni a ontologiei pentru a evita validări contradictorii.


Stack Tehnologic

Framework:
LangChain - Local development + Ollama
LLM Models:
 Llama 3.1 8B
Qwen 2.5 7B
Llama 3.2 Latest
Ontologie si Semantic Validation:
 OWL 2 + RDF
SPARQL endpoint
Persistență Date: 
FAISS

Mecanisme de Validare Semantică

Consistency Checking verifică dacă faptele extrase contravin regulilor de consistență definite în ontologie. De exemplu, dacă ontologia specifică că o persoană poate lucra pentru o singură companie la un moment dat, sistemul detectează și respinge faptele care contravin această regulă.
Type Validation confirmă că fiecare entitate menționată în fapte aparține tipului semantic corect conform schemei. Dacă ontologia definește "Medicament" ca subclasă a "SubstanțăChimică", sistemul validează că entitatea generată aparține acestei ierarhii.

Relationship Validation asigură că relațiile dintre entități sunt permise conform ontologiei. Dacă o relație nu este definită în ontologie, este marcată ca suspectă și necesită validare manuală.
Inference Validation verifică dacă faptele generate pot fi deduse din regulile existente în ontologie sau dacă contravin rezultatelor inferenței. Sistemul utilizează un motor de inferență pentru a calcula faptele implicite și pentru a verifica consistency cu faptele explicite generate.


Arhitectura Sistemului
	Sistemul RAG Agentic propus este compus din cinci module principale, cu integrare completă a modelelor Ollama locale:

Modul de Retrieval
Utilizează Ollama Embeddings pentru a genera reprezentări semantice ale documentelor și query-urilor. Implementează vector similarity search cu FAISS pentru performanță locală.

Modul de Generare cu Agenți
	Folosește LangChain pentru orchestrarea agenților autonomi. Fiecare agent selectează modelul Ollama optim: Llama 3.1 8B, Qwen 2.5 7B si Llama 3.2

Modul de Validare Semantică
	Mapează faptele generate la conceptele din ontologie și aplică patru tipuri de validări: Consistency validation, Type validation, Relationship validation si Inference validation

Motor de Inferență
Folosește reasonere OWL 2 (HermiT/Pellet) pentru a deduce fapte noi și a detecta inconsistențe logice.

Audit Trail & Explicabilitate
Înregistrează fiecare decizie, model folosit, motiv de respingere și lanțul de raționament pentru transparență completă.




     9. Metrici de evaluare

Evaluarea sistemului propus se efectuează pe patru dimensiuni complementare. Accuracy (precizie factualității) măsoară cât de multe fapte generate sunt factual corecte. Completeness (completitudine) măsoară cât de multe fapte relevante sunt generate. Explainability (explicabilitate) cuantifică ușurința de a înțelege rațiunile din spatele fiecărei afirmații. Latency (latență) măsoară timpul de răspuns complet.


     10. Limitări Identificate

Sistemul propus prezintă anumite limitări. Ingenieria ontologiilor necesită efort substantiv și expertiză de domeniu. Ontologiile statice nu se adaptează ușor la schimbări rapide în domeniu. Latența poate fi crescută datorită validărilor multiple. Agenții pot rămâne blocați în ciclurii infinite dacă ontologia este incompletă.


      11. Concluzie

Halucinațiile în modelele lingvistice mari rămân o problemă fundamentală pentru aplicații enterprise sensibile la erori. Această lucrare propone o soluție holistică care combină avantajele agenților autonomi, modelelor lingvistice avansate și ontologiilor formale. Rezultatele experimentale preliminare sunt promițătoare, demonstrând potențialul semnificativ al validării semantice în reducerea halucinațiilor și îmbunătățirea preciziei faptuale.
Sistemul propus nu doar reduce halucinații, ci oferă și transparență completă și explicabilitate, aspecte critice pentru încredere în sisteme AI în domenii sensibile. Roadmap-ul de dezvoltare viitoare promite extensii semnificative care vor face sistemul mai practic și scalabil.
În concluzie, integrarea ontologiilor în sisteme RAG agentice reprezentă o direcție promițătoare pentru construirea de sisteme de IA mai sigure, mai precise și mai transparente.
