import os
from langchain_community.vectorstores import FAISS
from langchain_ollama import OllamaEmbeddings, OllamaLLM
from langchain_core.documents import Document
from rdflib import Graph
from owlrl import DeductiveClosure, OWLRL_Semantics
from langchain.agents import AgentExecutor, create_react_agent
from langchain.tools import Tool
from langchain.prompts import PromptTemplate
import re

# configurare ollama
ollama_model = "llama3.1:8b"  
print(f"Initializing Ollama LLM with model: {ollama_model}")
llm = OllamaLLM(model=ollama_model)

print(f"Initializing Ollama Embeddings with model: {ollama_model}")
embedding_model = OllamaEmbeddings(model=ollama_model)

# ia documents si le transforma in numere(embeddings) pe care le putem cauta rapid
def create_faiss_index(documents):
    docs = [Document(page_content=doc) for doc in documents]
    vector_store = FAISS.from_documents(docs, embedding_model)
    return vector_store

# functia standard RAG, cauta in faiss cele mai relevante documente si genereaza raspuns
def rag_query(query, vector_store, llm):
    # retrieve relevant documents
    docs = vector_store.similarity_search(query, k=3)
    context = "\n".join([doc.page_content for doc in docs])
    
    # create prompt
    prompt = f"""
    Context: {context}
    
    Question: {query}
    
    Answer based on the context provided:
    """
    
    # generate response
    response = llm.invoke(prompt)
    return response, docs

# incarcam fisierul ontologiei, folosim un motor de rationament DeductiveClosure 
# pentru a intelege relatiile si verifica daca lucrurile extrase din raspunsul AI
# exista in ontologie
# construieste un grafic rdf(resource description framework) care contine toate clasele, proprietatile si instantele definite in ontologie
def semantic_validation(facts, ontology_path=None):
    if ontology_path and os.path.exists(ontology_path):
        try:
            print(f"Loading ontology from: {ontology_path}")
            g = Graph()
            g.parse(ontology_path, format="xml")
            DeductiveClosure(OWLRL_Semantics).expand(g)

            validated_facts = []
            for fact in facts:
                # Convertesc fact la URIRef pentru verificare
                from rdflib import URIRef, Namespace
                ex = Namespace("http://example.org/rag_ontology#")
                
                subject_uri = ex[fact['subject']]
                predicate_uri = ex[fact['predicate']]
                object_uri = ex[fact['object']]
                
                if (subject_uri, predicate_uri, object_uri) in g:
                    validated_facts.append(fact)
                    print(f"Fact validated: {fact}")
                else:
                    print(f"Fact invalid: {fact}")
                    
            return validated_facts
        except Exception as e:
            print(f"Error loading ontology: {e}")
            return facts
    else:
        print("Ontology file not found, skipping validation")
        return facts

# agent tools pentru comportament autonom
def fact_extraction_tool(text: str) -> str:
    print(f"Extracting facts from: {text[:100]}...")
    
    # simuleaza extragerea de fapte folosind pattern matching simplu
    facts = []
    patterns = [
        r'(\w+)\s+(sunt|este|pot fi)\s+(\w+)',
        r'(\w+)\s+(utilizat|folosit)\s+pentru\s+(\w+)',
        r'(\w+)\s+(reduce|elimina|previne)\s+(\w+)'
    ]
    
    for pattern in patterns:
        matches = re.findall(pattern, text.lower())
        for match in matches:
            fact = {
                'subject': match[0],
                'predicate': match[1].replace(' ', '_'),
                'object': match[2]
            }
            facts.append(fact)
    
    return str(facts)

def semantic_validation_tool(facts_str: str) -> str:
    print(f"Validating facts: {facts_str[:100]}...")
    
    try:
        # parse facts din string
        facts = eval(facts_str) if facts_str.startswith('[') else []
        ontology_path = "/home/cowsmin/Documents/proiecte/cloud/simple_ontology.owl"
        validated = semantic_validation(facts, ontology_path)
        
        validation_result = {
            'total_facts': len(facts),
            'validated_facts': len(validated),
            'validation_rate': len(validated) / len(facts) if facts else 0,
            'details': validated
        }
        return f"Validation completed: {validation_result['validation_rate']:.2%} facts validated"
    except Exception as e:
        return f"Validation error: {str(e)}"

def hallucination_detection_tool(response: str) -> str:
    # detecteaza posibile halucinatii in raspuns
    print(f"Checking for hallucinations in response...")
    
    # Indicatori simpli de halucinatii
    hallucination_indicators = [
        'cu siguranta', 'definitiv', 'intotdeauna', 'niciodata',
        'toti expertii', 'studiile arata ca', 'conform cercetarilor recente'
    ]
    
    found_indicators = [ind for ind in hallucination_indicators if ind in response.lower()]
    
    if found_indicators:
        return f"Potential hallucination detected: {found_indicators}"
    else:
        return "No obvious hallucination indicators found"

# tools pentru agent
tools = [
    Tool(
        name="FactExtraction",
        func=fact_extraction_tool,
        description="Extract semantic facts from text for validation"
    ),
    Tool(
        name="SemanticValidation", 
        func=semantic_validation_tool,
        description="Validate extracted facts against ontology"
    ),
    Tool(
        name="HallucinationDetection",
        func=hallucination_detection_tool,
        description="Detect potential hallucinations in generated text"
    )
]

# agent prompt template
agent_prompt = PromptTemplate.from_template("""
You are a semantic validation agent for a RAG system. Your job is to:
1. Extract facts from generated responses
2. Validate them against an ontology 
3. Detect potential hallucinations

You have access to the following tools:
{tools}

Tool names: {tool_names}

Question: {input}
Generated Response: {agent_scratchpad}

Think step by step and use the tools to validate the response.
""")

# creez agentul
agent = create_react_agent(llm, tools, agent_prompt)
agent_executor = AgentExecutor(agent=agent, tools=tools, verbose=True, max_iterations=3)

# functie imbunatatita pentru RAG cu validare agentica
def agentic_rag_query(query, vector_store, llm, agent_executor):
    print(f"\nStarting Agentic RAG Pipeline...")
    
    # Step 1: Standard RAG
    print("Phase 1: Document Retrieval & Generation")
    response, docs = rag_query(query, vector_store, llm)
    
    # Step 2: Agentic Validation
    print("Phase 2: Agentic Semantic Validation")
    try:
        validation_input = {
            "input": query,
            "agent_scratchpad": response
        }
        validation_result = agent_executor.invoke(validation_input)
        
        return {
            'original_response': response,
            'source_documents': docs,
            'validation_result': validation_result,
            'agent_output': validation_result.get('output', 'No agent output')
        }
    except Exception as e:
        print(f"Agent validation failed: {e}")
        return {
            'original_response': response,
            'source_documents': docs,
            'validation_result': f"Validation failed: {e}",
            'agent_output': 'Agent validation unavailable'
        }

# o lista de stringuri, iar faiss cauta doar in aceste propozitii cand punem o intrebare
documents = [
    "Ontologiile sunt utilizate pentru validarea semantica.",
    "Modelele lingvistice mari pot genera halucinatii.",
    "SPARQL este folosit pentru interogarea bazelor de date RDF.",
    "RAG combina retrieval cu generare pentru raspunsuri mai precise.",
    "Azure Semantic Kernel ofera orchestrare pentru agenti AI."
]

# Creare index FAISS
try:
    print("Creating FAISS index...")
    vector_store = create_faiss_index(documents)
    print("FAISS index created successfully")
except Exception as e:
    print(f"Error creating FAISS index: {e}")
    exit(1)

# Adauga comentarii explicative pentru fiecare componenta
print("=== COMPONENTE SISTEM RAG AGENTIC ===")

# 1. RETRIEVAL - Gaseste documente relevante
print("\n1. RETRIEVAL COMPONENT:")
print("- Foloseste FAISS pentru cautare vectoriala")
print("- Gaseste documentele cele mai similare cu query-ul")

# 2. GENERATION - Genereaza raspuns cu LLM
print("\n2. GENERATION COMPONENT:")
print("- Foloseste Ollama cu model local llama3.1:8b")
print("- Primeste context din documente + intrebare")
print("- Genereaza raspuns bazat pe context")

# 3. ONTOLOGY VALIDATION - Valideaza semantic
print("\n3. ONTOLOGY VALIDATION:")
print("- Ar trebui sa valideze faptele generate cu ontologii OWL/RDF")
print("- Detecteaza inconsistente si halucinatii")
print("- Asigura corectitudine semantica")

# 4. AGENTIC BEHAVIOR - Comportament autonom
print("\n4. AGENTIC BEHAVIOR (LIPSESTE):")
print("- Agenti autonomi care iau decizii")
print("- Orchestrare cu Azure Semantic Kernel/LangChain")
print("- Planificare si executie automata")

print("\n" + "="*50)

# Exemplu de interogare cu sistem complet
query = "Cum pot fi utilizate ontologiile pentru a reduce halucinatii?"
try:
    print(f"\nSTARTING COMPLETE AGENTIC RAG DEMO")
    print(f"Query: {query}")
    
    result = agentic_rag_query(query, vector_store, llm, agent_executor)
    
    # Afisare rezultate complete
    print("\n" + "="*60)
    print("REZULTATE FINALE")
    print("="*60)
    
    print(f"\nIntrebare: {query}")
    
    print(f"\nDocumente relevante gasite:")
    for i, doc in enumerate(result['source_documents'], 1):
        print(f"{i}. {doc.page_content}")
    
    print(f"\nRaspuns generat de model:")
    print(result['original_response'])
    
    print(f"\nRezultat validare agentica:")
    print(result['agent_output'])
    
    print(f"\nValidare semantica:")
    print(result['validation_result'])

except Exception as e:
    print(f"Error during agentic query execution: {e}")
    import traceback
    traceback.print_exc()

def interactive_demo():
    """Ruleaza demo interactiv in consola"""
    print("\n" + "="*70)
    print("RAG AGENTIC INTERACTIV CU ONTOLOGII")
    print("="*70)
    print("Comenzi disponibile:")
    print("- Scrie o intrebare pentru a o procesa")
    print("- 'help' pentru ajutor")
    print("- 'stats' pentru statistici")
    print("- 'exit' pentru a iesi")
    print("="*70)
    
    stats = {
        'total_queries': 0,
        'successful_queries': 0,
        'failed_queries': 0
    }
    
    while True:
        print("\n" + "-"*50)
        query = input("Intrebare: ").strip()
        
        if query.lower() in ['exit', 'quit', 'stop', 'q']:
            print("Demo terminat!")
            print(f"Statistici finale: {stats['total_queries']} intrebari, {stats['successful_queries']} reusite")
            break
            
        elif query.lower() == 'help':
            print("""
AJUTOR:
- Intrebari sugerate:
  * "Cum pot fi utilizate ontologiile pentru a reduce halucinatiile?"
  * "Ce rol au agentii in sistemele RAG?"
  * "Cum functioneaza validarea semantica?"
- Comenzi:
  * 'stats' - Vezi statistici
  * 'exit' - Iesi din demo
            """)
            continue
            
        elif query.lower() == 'stats':
            print(f"""
STATISTICI DEMO:
- Total intrebari: {stats['total_queries']}
- Reusite: {stats['successful_queries']}
- Esuate: {stats['failed_queries']}
- Rata de succes: {(stats['successful_queries']/max(stats['total_queries'],1)*100):.1f}%
- Documente in index: {len(documents)}
- Model folosit: {ollama_model}
            """)
            continue
            
        elif not query:
            print("Te rog introdu o intrebare valida!")
            continue
            
        # Proceseaza query-ul
        stats['total_queries'] += 1
        
        try:
            print(f"\nProcesez: '{query}'")
            print("Loading...")
            
            result = agentic_rag_query(query, vector_store, llm, agent_executor)
            
            # Afisare rezultate frumos formatate
            print("\n" + "="*60)
            print("                    REZULTATE RAG AGENTIC")
            print("="*60)
            
            print(f"\nINTREBARE:")
            print(f"   {query}")
            
            print(f"\nDOCUMENTE RELEVANTE GASITE:")
            for i, doc in enumerate(result['source_documents'], 1):
                print(f"   {i}. {doc.page_content}")
            
            print(f"\nRASPUNS GENERAT DE MODEL:")
            # Formatare frumoasa a raspunsului
            response_lines = result['original_response'].split('\n')
            for line in response_lines:
                if line.strip():
                    print(f"   {line.strip()}")
            
            print(f"\nVALIDARE AGENTICA:")
            agent_lines = str(result['agent_output']).split('\n')
            for line in agent_lines:
                if line.strip():
                    print(f"   {line.strip()}")
            
            print(f"\nSTATUS VALIDARE:")
            print(f"   {result.get('validation_result', 'N/A')}")
            
            print("\n" + "="*60)
            
            stats['successful_queries'] += 1
            
        except Exception as e:
            print(f"\nEROARE: {e}")
            print("Sugestie: Verifica ca Ollama ruleaza: 'ollama serve'")
            stats['failed_queries'] += 1

if __name__ == "__main__":
    print("\nDEMO RAG AGENTIC COMPLET EXECUTAT")
    
    # apoi ofera optiunea pentru demo interactiv
    print("\n" + "="*60)
    print("Vrei sa continui cu demo-ul interactiv?")
    print("   - Poti pune intrebari multiple")
    print("   - Vezi statistici in timp real")
    print("   - Interfata user-friendly in terminal")
    print("="*60)
    
    choice = input("Continui cu demo interactiv? (y/n): ").lower()
    if choice in ['y', 'yes', 'da', 'sure', '1']:
        interactive_demo()
    else:
        print("Demo terminat! Ruleaza din nou pentru a continua.")