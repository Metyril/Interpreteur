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

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
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
// NoeudChaine
////////////////////////////////////////////////////////////////////////////////

NoeudChaine::NoeudChaine(Noeud* chaine) 
:m_chaine(chaine){

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
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(vector<Noeud*> aecrire)
:m_aecrire(aecrire){}


int NoeudInstEcrire::executer() {
   for(int i = 0; i < m_aecrire.size(); i++){
            //cout << m_sequence;
       if (typeid(*m_aecrire[i])==typeid(NoeudChaine)){
            m_aecrire[i]->executer();
            cout <<" ";
       }else {
            cout << (m_aecrire[i]->executer()) << " ";
       }
    } 
        

    return 0; // La valeur renvoyée ne représente rien !
}