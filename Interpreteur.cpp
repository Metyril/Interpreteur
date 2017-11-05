#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <valarray>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {}


void Interpreteur::traduitEnCPP(ostream & cout,unsigned int indentation)const {
    cout << "#include <cstdlib>" << endl;
    cout << "#include <iostream>" << endl;
    cout << "using namespace std;" << endl << endl;

    cout << setw(4*indentation)<<""<<"int main() {"<< endl; // Début d’un programme C++

// Ecrire en C++ la déclaration des variables présentes dans le programme... 
// ... variables dont on retrouvera le nom en parcourant la table des symboles ! 
// Par exemple, si le programme contient i,j,k, il faudra écrire : int i; int j; int k; ... 
  for(auto &symbole : getTable().getListeSymboles()) {  // Parcours de la liste des symboles
      if(symbole->getCategorie() == 1) {                // On compare la catégorie du symbole à son rang dans m_categorie
                                                        // Un symbole au rang est une variable, on ne traite donc que ces symboles
          cout << setw(4*(indentation+1))<<""<<"int "<< symbole->getChaine() << ";" << endl;    // Initialisation des variables à la compilation
      }
  }
  getArbre()->traduitEnCPP(cout,indentation+1); // Lance l'opération traduitEnCPP sur la racine
  cout << endl;
  cout << setw(4*(indentation+1))<<""<<"return 0;"<< endl;
  cout << setw(4*indentation)<<"}" << endl;     // Fin d’un programme C++
}


void Interpreteur::analyse() {
  m_arbre = programme();    // On lance l'analyse de la première règle
}


void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) {
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(),
            symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
  }
}


void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
  tester(symboleAttendu);
  m_lecteur.avancer();
}


void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
  throw SyntaxeException(messageWhat);
}


Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  return sequence;
}


Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" ||
           m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" ||
           m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "lire" ||
           m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "selon");
  // Tant que le symbole courant est un début possible d'instruction...
  return sequence;
}


Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation> ; | <instSiRiche> | <instTantQue> | <instRepeter> ; | <instPour> | <instEcrire> ; | <instLire> ; | <instSelon>
    try {
        if (m_lecteur.getSymbole() == "<VARIABLE>") {
            Noeud *affect = affectation();
            testerEtAvancer(";");
            return affect;
        }
        else if (m_lecteur.getSymbole() == "si")
            return instSiRiche();
        else if (m_lecteur.getSymbole() == "tantque")
            return instTantQue();
        else if (m_lecteur.getSymbole() == "repeter")
            return instRepeter();
        else if (m_lecteur.getSymbole() == "pour")
            return instPour();
        else if (m_lecteur.getSymbole() == "lire")
            return instLire();
        else if (m_lecteur.getSymbole() == "ecrire")
            return instEcrire();
        else if (m_lecteur.getSymbole() == "selon")
            return instSelon();
        else {
            erreur("Instruction incorrecte");
        }
    } catch(SyntaxeException) {
      // Tant que le symbole n'est pas un début d'instruction ...
        while(m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "si" &&
              m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" &&
              m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "lire" &&
              m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "selon") {
            m_lecteur.avancer();    // ... on avance dans le code.
        }
    return nullptr;     // On retourne un noeud vide pour continuer l'exécution
    }
}


Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression> 
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
    m_lecteur.avancer();
    testerEtAvancer("=");
    Noeud* exp = expression();             // On mémorise l'expression trouvée
    return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}


Noeud* Interpreteur::expression() {
    // <expression> ::= <terme> { + <terme> | - <terme> }
    Noeud* term = terme();      // On mémorise le premier terme
    while (m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-") {
        Symbole operateur = m_lecteur.getSymbole();     // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* termDroit = terme();     // On mémorise le deuxième terme
        term = new NoeudOperateurBinaire(operateur, term, termDroit);       // Et on construit un noeud opérateur binaire
    }
    return term; // On retourne le terme qui pointe sur l'expression
}


Noeud* Interpreteur::terme() {
    // <terme> ::= <facteur> { * <facteur> | / <facteur> }
    Noeud* fact = facteur();      // On mémorise le premier facteur
    while (m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/") {
        Symbole operateur = m_lecteur.getSymbole();     // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* factDroit = facteur();       // On mémorise le deuxième facteur
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit);       // Et on construit un noeud opérateur binaire
    }
    return fact; // On retourne le facteur qui pointe sur le terme
}


Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | <chaine> | - <expBool> | non <expBool> | ( <expBool> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<ENTIER>" || m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<CHAINE>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <expBool>
        m_lecteur.avancer();
        // on représente le moins unaire (- expBool) par une soustraction binaire (0 - expBool)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), expBool());
    } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("non"), expBool(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expBool();
        testerEtAvancer(")");
    } else 
        erreur("Facteur incorrect");
    return fact;
}


Noeud* Interpreteur::expBool() {
    // <expBool> ::= <relationET> { ou <relationEt> }
    Noeud* relEt = relationEt();      // On mémorise la première relation ET
    while (m_lecteur.getSymbole() == "ou") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* relEtDroit = relationEt(); // On mémorise la deuxième relation ET
        relEt = new NoeudOperateurBinaire(operateur, relEt, relEtDroit); // Et on construit un noeud opérateur binaire
    }
    return relEt; // On retourne la relation ET qui pointe sur l'expression booléenne
}


Noeud* Interpreteur::relationEt() {
    // <relationEt> ::= <relation> { et <relation> }
    Noeud* rel = relation();      // On mémorise la première relation
    while (m_lecteur.getSymbole() == "et") {
        Symbole operateur = m_lecteur.getSymbole();     // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* relDroit = relation();       // On mémorise la deuxième relation
        rel = new NoeudOperateurBinaire(operateur, rel, relDroit);      // Et on construit un noeud opérateur binaire
    }
    return rel; // On retourne la relation qui pointe sur la relation ET
}


Noeud* Interpreteur::relation() {
    // <relation> ::= <expression> { <opRel> <expression> }
    //  <opRel> ::=   == | != | < | <= | > | >=
    Noeud* exp = expression();      // On mémorise la première expression
    while ( m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
            m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
            m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ) {
        Symbole operateur = m_lecteur.getSymbole();     // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* expDroit = expression();     // On mémorise la deuxième expression
        exp = new NoeudOperateurBinaire(operateur, exp, expDroit);      // Et on construit un noeud opérateur binaire
    }
    return exp; // On retourne l'expression qui pointe sur la relation
}


Noeud* Interpreteur::instSiRiche() {
    //<instSiRiche> ::= si ( <expBool> ) <seqInst> { sinonsi ( <expBool> ) <seqInst> } [ sinon ( <expBool> ) <seqInst> ] finsi
    vector <Noeud* > conditions;    // Contient la liste des conditions du SiRiche
    vector <Noeud* > sequences;     // Contient la liste des séquences à exécuter
    
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud* condition = expBool();       // On mémorise la condition
    conditions.push_back(condition);
    testerEtAvancer(")");
    Noeud* sequence = seqInst();        // On mémorise la séquence d'instruction
    sequences.push_back(sequence);
    
    while(m_lecteur.getSymbole() == "sinonsi"){
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        Noeud* condition = expBool();   // On mémorise la condition
        conditions.push_back(condition);
        testerEtAvancer(")");
        Noeud* sequence = seqInst();    // On mémorise la séquence d'instruction
        sequences.push_back(sequence);
    }
    
    if(m_lecteur.getSymbole() == "sinon"){
        testerEtAvancer("sinon");
        conditions.push_back(nullptr);
        Noeud* sequence = seqInst();    // On mémorise la séquence d'instruction
        sequences.push_back(sequence);
    }
    
    testerEtAvancer("finsi");
    return new NoeudInstSiRiche(conditions, sequences);
}


Noeud* Interpreteur::instTantQue() {
    //<instTantQue> ::= tantque ( <expBool> ) <seqInst> fintantque
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expBool();   // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst();    // On mémorise la séquence d'instruction
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
}


Noeud* Interpreteur::instRepeter() {
    //<instRepeter> ::= repeter <seqInst> jusqua( <expBool> )
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();    // On mémorise la séquence d'instruction
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expBool();   // On mémorise la condition
    testerEtAvancer(")");
    return new NoeudInstRepeter(condition, sequence);
}


Noeud* Interpreteur::instPour(){
    // <instPour>   ::= pour( [ <affectation> ] ; <expBool> ; [ <affectation> ] ) <seqInst> finpour
    // On définit les affectations du pour comme nulles par défaut
    Noeud* affect1 = nullptr;
    Noeud* affect2 = nullptr;
    
    testerEtAvancer("pour");
    testerEtAvancer("(");
    
    if (m_lecteur.getSymbole() == ";"){
        testerEtAvancer(";");
    } else {
        affect1 = affectation();    // On mémorise la première affectation
        testerEtAvancer(";");
    }
    
    Noeud* condition = expBool();   // On mémorise la condition
    testerEtAvancer(";");
    
    if (m_lecteur.getSymbole() != "<VARIABLE>"){
        testerEtAvancer(")");
    } else {
        affect2 = affectation();    // On mémorise la deuxième affectation
        testerEtAvancer(")");
    }

    Noeud* sequence = seqInst();    // On mémorise la séquence d'instruction
    testerEtAvancer("finpour");    
    return new NoeudInstPour(condition, sequence, affect1, affect2);
}


Noeud* Interpreteur::instLire() {
    // <instLire> ::= lire( <variable> { , <variable> } )
    vector <Noeud* > variables;     // Contient la liste des variables à traiter

    testerEtAvancer("lire");
    testerEtAvancer("(");
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
    variables.push_back(var);       // On mémorise la variable
    m_lecteur.avancer();
    
    while(m_lecteur.getSymbole() == ","){
        testerEtAvancer(",");
        tester("<VARIABLE>");
        var = m_table.chercheAjoute(m_lecteur.getSymbole());
        variables.push_back(var);   // On mémorise la variable
        m_lecteur.avancer();
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstLire(variables);
}


Noeud* Interpreteur::instEcrire() {
    // <instEcrire> ::=ecrire( <expBool> | <chaine> {, <expBool> | <chaine> })    
    vector <Noeud* > aecrire;       // Contient la liste des éléments à écrire
    
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    
    if(m_lecteur.getSymbole() == "<CHAINE>" || m_lecteur.getSymbole() == "<ENTIER>" ){
        NoeudChaine* chaine = new NoeudChaine(expBool());
        aecrire.push_back(chaine);      // On mémorise la chaîne
    } else if (m_lecteur.getSymbole() == "<VARIABLE>" ){
        Noeud* expr = expBool();
        aecrire.push_back(expr);    // On mémorise l'expression
    }
    
    while(m_lecteur.getSymbole() != ")"){
        testerEtAvancer(",");
       if(m_lecteur.getSymbole() == "<CHAINE>" || m_lecteur.getSymbole() == "<ENTIER>" ){
            NoeudChaine* chaine = new NoeudChaine(expBool());
            aecrire.push_back(chaine);      // On mémorise la chaîne
        } else if (m_lecteur.getSymbole() == "<VARIABLE>" ){
            Noeud* expr = expBool();
            aecrire.push_back(expr);    // On mémorise l'expression
        }
    }
    
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstEcrire(aecrire);
}


Noeud* Interpreteur::instSelon() {
    //<instSelon> ::= selon ( <expBool> ) {cas <expBool> : <seqInst>} [defaut <seqInst>] finselon
    vector <Noeud*> expressions;    // Contient les différentes expressions à comparer avec la variable
    vector <Noeud*> sequences;      // Contient la liste des séquences à exécuter

    testerEtAvancer("selon");
    testerEtAvancer("(");
    Noeud* variable = expBool();    // On mémorise la variable à traiter
    testerEtAvancer(")");
    
    while(m_lecteur.getSymbole() == "cas"){
        testerEtAvancer("cas");
        Noeud* exp = expBool();
        expressions.push_back(exp);     // On mémorise la condition
        testerEtAvancer(":");
        Noeud* sequence = seqInst();
        sequences.push_back(sequence);  // On mémorise la séquence d'instruction
    }
    
    if(m_lecteur.getSymbole() == "defaut"){
        testerEtAvancer("defaut");
        testerEtAvancer(":");
        expressions.push_back(nullptr);
        Noeud* sequence = seqInst();
        sequences.push_back(sequence);  // On mémorise la séquence d'instruction
    }
    
    testerEtAvancer("finselon");
    return new NoeudInstSelon(variable, expressions, sequences);
}
