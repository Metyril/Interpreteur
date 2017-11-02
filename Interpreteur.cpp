#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <valarray>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}



void Interpreteur::traduitEnCPP(ostream & cout,unsigned int indentation)const {
  cout << setw(4*indentation)<<""<<"int main() {"<< endl; // Début d’un programme C++
// Ecrire en C++ la déclaration des variables présentes dans le programme... 
// ... variables dont on retrouvera le nom en parcourant la table des symboles ! 
// Par exemple, si le programme contient i,j,k, il faudra écrire : int i; int j; int k; ... 
  getArbre()->traduitEnCPP(cout,indentation+1);// lance l'opération traduitEnCPP sur la racine
  cout << setw(4*(indentation+1))<<""<<"return 0;"<< endl ; 
  cout << setw(4*indentation)<<"}" << endl ; // Fin d’un programme C++
} 




void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
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
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "lire" || m_lecteur.getSymbole() == "ecrire" );
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
  try {
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
      Noeud *affect = affectation();
      testerEtAvancer(";");
      return affect;
    }
    else if (m_lecteur.getSymbole() == "si")
      return instSiRiche();
    // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
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
    else {
        erreur("Instruction incorrecte");
    }
  } catch(SyntaxeException) {
    while(m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "si" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "ecrire") {
        m_lecteur.avancer();
    }
    return nullptr;
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
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* fact = facteur();
  while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-"  ||
          m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"  ||
          m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
          m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
          m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"   ) {
    Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
    m_lecteur.avancer();
    Noeud* factDroit = facteur(); // On mémorise l'opérande droit
    fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
  }
  return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> ) | <chaine>
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>" || m_lecteur.getSymbole() == "<CHAINE>") {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else 
    erreur("Facteur incorrect");
  return fact;
}

Noeud* Interpreteur::instSi() {
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
  testerEtAvancer("si");
  testerEtAvancer("(");
  Noeud* condition = expression(); // On mémorise la condition
  testerEtAvancer(")");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  testerEtAvancer("finsi");
  return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
}










Noeud* Interpreteur::instTantQue() {
    //<instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
}



Noeud* Interpreteur::instRepeter() {
    //<instRepeter> ::=repeter <seqInst> jusqua( <expression> )
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    return new NoeudInstRepeter(condition, sequence);
}


Noeud* Interpreteur::instPour(){
    // <instPour>   ::=pour( [ <affectation> ] ; <expression> ;[ <affectation> ]) <seqInst> finpour
    Noeud* affect1 = nullptr;
    Noeud* affect2 = nullptr;
    
    testerEtAvancer("pour");
    testerEtAvancer("(");
    
    if (m_lecteur.getSymbole() == ";"){
        
        testerEtAvancer(";");
        
    } else {

        affect1 = affectation();
        testerEtAvancer(";");
    }
    
    
    Noeud* condition = expression();
    testerEtAvancer(";");
    
    if (m_lecteur.getSymbole() != "<VARIABLE>"){
        testerEtAvancer(")");
    } else {
        affect2 = affectation();
        testerEtAvancer(")");
    }

    
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    
    return new NoeudInstPour(condition, sequence, affect1, affect2);
}



Noeud* Interpreteur::instSiRiche() {
    vector <Noeud* > conditions;
    vector <Noeud* > sequences;
    
    //<instSiRiche> ::= si ( <expression> ) <seqInst> {sinonsi ( <expression> ) <seqInst>} [sinon ( <expression> ) <seqInst>] finsi
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    conditions.push_back(condition);
    testerEtAvancer(")");
    Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
    sequences.push_back(sequence);
    
    while(m_lecteur.getSymbole() == "sinonsi"){
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        Noeud* condition = expression(); // On mémorise la condition
        conditions.push_back(condition);
        testerEtAvancer(")");
        Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
        sequences.push_back(sequence);
    }
    
    if(m_lecteur.getSymbole() == "sinon"){
        testerEtAvancer("sinon");
        conditions.push_back(nullptr);
        Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
        sequences.push_back(sequence);
    }
    
    testerEtAvancer("finsi");
    return new NoeudInstSiRiche(conditions, sequences);
}   

Noeud* Interpreteur::instLire() {
    // <instLire> ::= lire( <variable> {, <variable> })
    vector <Noeud* > variables;

    testerEtAvancer("lire");
    testerEtAvancer("(");
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
    variables.push_back(var);
    m_lecteur.avancer();
    
    while(m_lecteur.getSymbole() == ","){
        testerEtAvancer(",");
        tester("<VARIABLE>");
        var = m_table.chercheAjoute(m_lecteur.getSymbole());
        variables.push_back(var);
        m_lecteur.avancer();
    }
    testerEtAvancer(")");
    return new NoeudInstLire(variables);
}

Noeud* Interpreteur::instEcrire() {
    vector <Noeud* > aecrire;
    
    // <instEcrire> ::=ecrire( <expression> | <chaine> {, <expression> | <chaine> })    
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    
    
    if(m_lecteur.getSymbole() == "<CHAINE>" || m_lecteur.getSymbole() == "<ENTIER>" ){
        NoeudChaine* chaine = new NoeudChaine(expression()); // On mémorise la chaine
        aecrire.push_back(chaine);
    } else if (m_lecteur.getSymbole() == "<VARIABLE>" ){
        Noeud* expr = expression(); // On mémorise la condition
        aecrire.push_back(expr);
    }
    
    while(m_lecteur.getSymbole() != ")"){
        testerEtAvancer(",");
       if(m_lecteur.getSymbole() == "<CHAINE>" || m_lecteur.getSymbole() == "<ENTIER>" ){
            NoeudChaine* chaine = new NoeudChaine(expression()); // On mémorise la chaine
            aecrire.push_back(chaine);
        } else if (m_lecteur.getSymbole() == "<VARIABLE>" ){
            Noeud* expr = expression(); // On mémorise la condition
            aecrire.push_back(expr);
        }
    }
    
    testerEtAvancer(")");
    return new NoeudInstEcrire(aecrire);
}
