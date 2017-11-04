#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() = 0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
    virtual void traduitEnCPP(ostream & cout,unsigned int indentation) const {}
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();    // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction);  // Ajoute une instruction à la séquence
    void traduitEnCPP(ostream & cout,unsigned int indentation) const;

  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void traduitEnCPP(ostream & cout,unsigned int indentation) const;

  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();            // Exécute (évalue) l'opération binaire)
    void traduitEnCPP(ostream & cout,unsigned int indentation) const;

  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSi(Noeud* condition, Noeud* sequence);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout,unsigned int indentation) const;

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};











////////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction tantque"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
     // Construit une "instruction tantque" avec sa condition et sa séquence d'instruction
   ~NoeudInstTantQue() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction ecrire en C++

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};


////////////////////////////////////////////////////////////////////////////////
class NoeudInstSiRiche : public Noeud {
// Classe pour représenter un noeud "instruction siRiche"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSiRiche(vector<Noeud* > conditions, vector<Noeud* > sequences);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSiRiche() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction ecrire en C++

  private:
    vector<Noeud* >  m_conditions;
    vector<Noeud* >  m_sequences;
};




////////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter : public Noeud {
// Classe pour représenter un noeud "instruction repeter"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstRepeter(Noeud* condition, Noeud* sequence);
     // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction
   ~NoeudInstRepeter() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction ecrire en C++

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};


////////////////////////////////////////////////////////////////////////////////
class NoeudInstPour : public Noeud {
// Classe pour représenter un noeud "instruction pour"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* affect = nullptr, Noeud* affectation = nullptr);
     // Construit une "instruction pour" avec sa condition et sa séquence d'instruction
   ~NoeudInstPour() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction ecrire en C++

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
    Noeud*  m_affec1;
    Noeud*  m_affec2;
};


////////////////////////////////////////////////////////////////////////////////
class NoeudInstEcrire : public Noeud {
// Classe pour représenter un noeud "instruction ecrire"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstEcrire(vector<Noeud* > aecrire);
     // Construit une "instruction ecrire" avec sa condition et sa séquence d'instruction
   ~NoeudInstEcrire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction ecrire en C++

  private:
    vector<Noeud*>  m_aecrire;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstLire : public Noeud {
// Classe pour représenter un noeud "instruction lire"
  public:
    NoeudInstLire(vector<Noeud*> variables);
     // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction
   ~NoeudInstLire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction lire
    void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction ecrire en C++

  private:
    vector<Noeud*>  m_variables;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudChaine : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudChaine(Noeud* chaine); // construit une affectation
    ~NoeudChaine() {} // A cause du destructeur virtuel de la classe Noeud
    void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction ecrire en C++
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable

  private:
    Noeud* m_chaine;
};



////////////////////////////////////////////////////////////////////////////////
class NoeudInstSelon : public Noeud {
// Classe pour représenter un noeud "instruction siRiche"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSelon(Noeud* variable, vector<Noeud*> expressions, vector<Noeud*> sequences);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
    ~NoeudInstSelon() {} // A cause du destructeur virtuel de la classe Noeud
    void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction selon en C++
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    //void traduitEnCPP(ostream & cout, unsigned int indentation) const; //traduit la fonction ecrire en C++

  private:
    Noeud* m_variable;
    vector<Noeud*>  m_expressions;
    vector<Noeud*>  m_sequences;
};
#endif /* ARBREABSTRAIT_H */
