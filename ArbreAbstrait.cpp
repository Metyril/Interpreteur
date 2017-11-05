#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <typeinfo> 

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnCPP(ostream & cout,unsigned int indentation) const {
    for(auto &inst : m_instructions) {
        inst->traduitEnCPP(cout, indentation);
        cout << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(ostream & cout,unsigned int indentation) const {
  cout << setw(4*indentation)<<"";
  m_variable->traduitEnCPP(cout,indentation);
  cout << " = ";
  m_expression->traduitEnCPP(cout, indentation);
  cout << ";";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout,unsigned int indentation) const {
  m_operandeGauche->traduitEnCPP(cout,indentation);
  cout << " " << m_operateur.getChaine() << " ";
  m_operandeDroit->traduitEnCPP(cout, indentation);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitEnCPP(ostream & cout,unsigned int indentation) const {
  cout << setw(4*indentation)<<""<<"if (";  // Ecrit "if (" avec un décalage de 4*indentation espaces 
  m_condition->traduitEnCPP(cout,0);    // Traduit la condition en C++ sans décalage 
  cout <<") {"<< endl;  // Ecrit ") {" et passe à la ligne 
  m_sequence->traduitEnCPP(cout, indentation+1);    // Traduit en C++ la séquence avec indentation augmentée 
  cout << setw(4*indentation)<<""<<"}";  // Ecrit "}" avec l'indentation initiale et passe à la ligne
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}


int NoeudInstTantQue::executer() {
  while (m_condition->executer())
      m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"while (";// Ecrit "while (" avec un décalage de 4*indentation espaces 
    m_condition->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
    cout <<") {"<< endl;// Ecrit ") {" et passe à la ligne   
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation)<<""<<"}";// Ecrit "}" avec l'indentation initiale et passe à la ligne
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche(vector<Noeud*> conditions, vector<Noeud*> sequences)
: m_conditions(conditions), m_sequences(sequences){

}


int NoeudInstSiRiche::executer() {
    for(int i = 0; i < m_conditions.size(); i++){
        if(m_conditions[i] != nullptr ? m_conditions[i]->executer() : 1){
            m_sequences[i]->executer();
            break;
        }
    }
    
  return 0; // La valeur renvoyée ne représente rien !
}


void NoeudInstSiRiche::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"if (";// Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_conditions[0]->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
    cout <<") {"<< endl;// Ecrit ") {" et passe à la ligne   
    m_sequences[0]->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation)<<""<<"}";// Ecrit "}" avec l'indentation initiale et passe à la ligne
    
    for(int i = 1; i < m_conditions.size(); i++){
        if(m_conditions[i] != nullptr){
            cout << " else if (";// Ecrit "if (" avec un décalage de 4*indentation espaces 
            m_conditions[i]->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
            cout <<") {"<< endl;// Ecrit ") {" et passe à la ligne   
            m_sequences[i]->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
            cout << setw(4*indentation)<<""<<"}";// Ecrit "}" avec l'indentation initiale et passe à la ligne
        } else {
            cout << " else {"<<endl;// Ecrit "else {" avec un décalage de 4*indentation espaces et passe à la ligne   
            m_sequences[i]->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
            cout << setw(4*indentation)<<""<<"}";// Ecrit "}" avec l'indentation initiale et passe à la ligne
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}


int NoeudInstRepeter::executer() {
    do
        m_sequence->executer();
    while (!m_condition->executer());
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstRepeter::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"do {"<<endl;// Ecrit "do" avec un décalage de 4*indentation espaces 
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation)<<""<<"} while (!(";
    m_condition->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
    cout <<"));";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* affect, Noeud* affectation) 
:m_affec1(affect), m_condition(condition), m_affec2(affectation), m_sequence(sequence) {

}


int NoeudInstPour::executer() {
   for(m_affec1 != nullptr ? m_affec1->executer() : 0 ; m_condition->executer(); m_affec2 != nullptr ? m_affec2->executer() : 0 ){
            //cout << m_sequence;
            m_sequence->executer();
        } 
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstPour::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"for("; // Ecrit "for (" avec un décalage de 4*indentation espaces 
    if( m_affec1 != nullptr){
        m_affec1->traduitEnCPP(cout,0);//traduit la première affectation en C++
    }
    
    m_condition->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
    
    if( m_affec2 != nullptr){
        cout << ";";
        m_affec2->traduitEnCPP(cout,0);//traduit la première affectation en C++
        cout << "\b";
    }
    cout << ") {" << endl;
    
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation)<<""<<"}";// Ecrit "}" avec l'indentation initiale et passe à la ligne 

}


////////////////////////////////////////////////////////////////////////////////
// NoeudChaine
////////////////////////////////////////////////////////////////////////////////

NoeudChaine::NoeudChaine(Noeud* chaine) 
:m_chaine(chaine){

}

void NoeudChaine::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << ((SymboleValue *)m_chaine)->getChaine();
}


int NoeudChaine::executer() {
    /*if (typeid(*m_chaine)==typeid(SymboleValue) && *((SymboleValue*)m_chaine)== "<CHAINE>" ) {
        cout << ((SymboleValue *)m_chaine)->getChaine();
    }else if (typeid(*m_chaine)==typeid(SymboleValue) && *((SymboleValue*)m_chaine)== "<ENTIER>" ) {
        cout << ((SymboleValue *)m_chaine)->getChaine();
    }*/
    
    cout << ((SymboleValue *)m_chaine)->getChaine();

    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(vector<Noeud*> variables)
: m_variables(variables){
}


int NoeudInstLire::executer() {
    int valeur;
    for(int i = 0; i < m_variables.size(); i++) {
        cout << ((SymboleValue*) m_variables[i])->getChaine() << " : ";
        cin >> valeur;
        cout << endl;
        ((SymboleValue*) m_variables[i])->setValeur(valeur);
    }
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstLire::traduitEnCPP(ostream & cout, unsigned int indentation) const{

    for(int i =0; i<m_variables.size(); i++){
        cout << setw(4*indentation)<<""<<"cin >> ";
        m_variables[i]->traduitEnCPP(cout, 0);
        if (i != m_variables.size()-1) {
            cout << ";" << endl;
        } else {
            cout << ";";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(vector<Noeud*> aecrire)
:m_aecrire(aecrire){}


int NoeudInstEcrire::executer() {
   for(int i = 0; i < m_aecrire.size(); i++){
            //cout << m_sequence;
       if (typeid(*m_aecrire[i])==typeid(NoeudChaine)){
            m_aecrire[i]->executer();
            cout << " ";
       }else {
            cout << (m_aecrire[i]->executer()) << " ";
       }
    } 
        

    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstEcrire::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    
    cout << setw(4*indentation)<< ""<<"cout"; // Ecrit "cout <<" avec un décalage de 4*indentation espaces
    for(int i =0; i<m_aecrire.size(); i++){
        cout << " << ";
        m_aecrire[i]->traduitEnCPP(cout, 0);
    }
    cout << " << endl;";
    
}



////////////////////////////////////////////////////////////////////////////////
// NoeudInstSelon
////////////////////////////////////////////////////////////////////////////////

NoeudInstSelon::NoeudInstSelon(Noeud* variable, vector<Noeud*> expressions, vector<Noeud*> sequences)
: m_variable(variable), m_expressions(expressions), m_sequences(sequences){

}


int NoeudInstSelon::executer() {
        
    for(int i = 0; i < m_expressions.size(); i++){
        if(m_expressions[i] == nullptr) {
            m_sequences[m_sequences.size()-1]->executer();
            break;
        } else if (m_expressions[i]->executer() == m_variable->executer()) {
            m_sequences[i]->executer();
            break;
        }
    }

  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSelon::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"switch (";// Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_variable->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
    cout <<") {"<< endl;// Ecrit ") {" et passe à la ligne   

    for(int i = 0; i < m_expressions.size(); i++){
        if (m_expressions[i] != nullptr) {
            cout << setw(4*(indentation+1)) << "" << "case ";// Ecrit "if (" avec un décalage de 4*indentation espaces 
            m_expressions[i]->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
            cout <<" : {" << endl;// Ecrit ") {" et passe à la ligne   
            m_sequences[i]->traduitEnCPP(cout, indentation+2);// Traduit en C++ la séquence avec indentation augmentée 
            cout << setw(4*(indentation+2)) << "" << "break;" << endl;
            cout << setw(4*(indentation+1)) << "" << "}" << endl; // Ecrit "}" avec l'indentation initiale et passe à la ligne
        } else {
            cout << setw(4*(indentation+1)) << "" << "default : ";
            m_sequences[i]->traduitEnCPP(cout,0);
        }
    }
    cout << setw(4*indentation) <<""<< "}";
    
}
