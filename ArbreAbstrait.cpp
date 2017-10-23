#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

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

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

void NoeudInstSi::traduitEnCPP(ostream cout("traduction.cpp"),unsigned int indentation) const{
    cout << setw(4*indentation) << "" << "if (";// Ecrit "if (" avec un décalage de 4*indentation espaces
    m_condition->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage
    cout << ") {" << endl;// Ecrit ") {" et passe à la ligne
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne    
}

int NoeudInstSi::executer() {
    if (m_condition->executer())
        m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !}
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

////////////////////////////////////////////////////////////////////////////////
// NoeudRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}


int NoeudInstRepeter::executer() {
    do
        m_sequence->executer();
    while (m_condition->executer());
  return 0; // La valeur renvoyée ne représente rien !
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
            cout << "fin";
        } 
        

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
        cout << "Variable : ";
        cin >> valeur;
        cout << endl;
        ((SymboleValue*) m_variables[i])->setValeur(valeur);
    }
  return 0; // La valeur renvoyée ne représente rien !
}