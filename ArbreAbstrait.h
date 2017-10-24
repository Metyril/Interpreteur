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
<<<<<<< HEAD
   NoeudInstSi(Noeud* condition, Noeud* sequence);
   // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
   //void traduitEnCPP(ostream & cout, unsigned int indentation) const;
   int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
//   void traduitEnCPP(ostream cout("traduction.cpp"),unsigned int indentation) const;
=======
    NoeudInstSi(Noeud* condition, Noeud* sequence);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};











////////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction tantque"
<<<<<<< HEAD
=======
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099
  public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
     // Construit une "instruction tantque" avec sa condition et sa séquence d'instruction
   ~NoeudInstTantQue() {} // A cause du destructeur virtuel de la classe Noeud
<<<<<<< HEAD
    int executer();  // Exécute l'instruction tantque
=======
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};


////////////////////////////////////////////////////////////////////////////////
class NoeudInstSiRiche : public Noeud {
// Classe pour représenter un noeud "instruction siRiche"
<<<<<<< HEAD
=======
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099
  public:
    NoeudInstSiRiche(vector<Noeud* > conditions, vector<Noeud* > sequences);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSiRiche() {} // A cause du destructeur virtuel de la classe Noeud
<<<<<<< HEAD
    int executer();  // Exécute l'instruction siRiche
=======
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099

  private:
    vector<Noeud* >  m_conditions;
    vector<Noeud* >  m_sequences;
};




////////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter : public Noeud {
// Classe pour représenter un noeud "instruction repeter"
<<<<<<< HEAD
=======
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099
  public:
    NoeudInstRepeter(Noeud* condition, Noeud* sequence);
     // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction
   ~NoeudInstRepeter() {} // A cause du destructeur virtuel de la classe Noeud
<<<<<<< HEAD
    int executer();  // Exécute l'instruction repeter
=======
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};


////////////////////////////////////////////////////////////////////////////////
class NoeudInstPour : public Noeud {
// Classe pour représenter un noeud "instruction pour"
<<<<<<< HEAD
=======
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099
  public:
    NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* affect = nullptr, Noeud* affectation = nullptr);
     // Construit une "instruction pour" avec sa condition et sa séquence d'instruction
   ~NoeudInstPour() {} // A cause du destructeur virtuel de la classe Noeud
<<<<<<< HEAD
    int executer();  // Exécute l'instruction pour
=======
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
    Noeud*  m_affec1;
    Noeud*  m_affec2;
};


////////////////////////////////////////////////////////////////////////////////
<<<<<<< HEAD
class NoeudInstLire : public Noeud {
// Classe pour représenter un noeud "instruction lire"
  public:
    NoeudInstLire(vector<Noeud*> variables);
     // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction
   ~NoeudInstLire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction lire

  private:
    vector<Noeud*>  m_variables;
};

=======
class NoeudInstEcrire : public Noeud {
// Classe pour représenter un noeud "instruction ecrire"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstEcrire(vector<Noeud* > aecrire);
     // Construit une "instruction ecrire" avec sa condition et sa séquence d'instruction
   ~NoeudInstEcrire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence

  private:
    vector<Noeud*>  m_aecrire;
};



////////////////////////////////////////////////////////////////////////////////
class NoeudChaine : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudChaine(Noeud* chaine); // construit une affectation
    ~NoeudChaine() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable

  private:
    Noeud* m_chaine;
};
>>>>>>> ba3a01a8b78b76aa58cdd087e7bf1990609a5099
#endif /* ARBREABSTRAIT_H */
