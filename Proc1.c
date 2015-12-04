/**
 *Code fait par Walter Ferney CASTRO LOPEZ
 *ENSIMAG - INP, Grenoble. 2015
 */

#include "Proc1.h"

#include "temps.h"
#include "console.h"
#include "inttypes.h"
#include "string.h"
#include "cpu.h"
#include "stdbool.h"
#include "stdio.h"
#include "segment.h"
#include "ecran.h"

/**
 *Fonction externe pour le changement de contexte
 */
extern void ctx_sw(uint32_t ancien [5], uint32_t vouveau[5]);

/**
 *Taille maximal de la pile
 */
#define TAILLE 512

/**
 *Nombre max de processus à exécuter
 */
#define MAXPROCESSUS 5

/**
 *Structure avec l'information du processus:
 *Son PID, so nom, son ETAT, ses registres; sa pile et son heure de reveille
 */
typedef struct processus {
  int PID;
  char * nom;
  int etat; //0 elu, 1 activable, 2 endormi, 3 Mort
  uint32_t registres [5];
  uint32_t pile [TAILLE];
  int heure;
}processus1;

struct processus table [MAXPROCESSUS]; //On crée une table de processus
struct processus * Actif; //On crée un pointeur aux processus actif
int NbProc = 1; //on initialialise le nombre de processus à 1 tenant en compte on aura toujours le processus IDLE

/**
 *Fonction d'ordonnance de processus.
 *Doit savoir quel processus exécuter et quand l'exécuter, 
 *tenant en compte son état et PID
 */
void ordonnance(void) {
  struct processus * Ancien; //On crée un pointeur vers le processus antient
  Ancien = Actif; //On initialise le processus antient comme le processus actuel
  int pidSuivant = Actif->PID+1; //On parcour la table de processus avec son PID, donc on recupère le PID du processus en exécution plus 1
  bool procSuivant = false; //Boolean disant si l'on a trouvé un nouveaux processus à exécuter
  
  int j = 0;
  for (int i=pidSuivant; i<(NbProc+Actif->PID); i++) { //Parcourir les processus qui reste, disant aue ce n'est pas le cas si l'on retourne sur le même processus aui est en exécution
    if (i >= NbProc) {
      j = i - NbProc;
    }
    else {
      j = i;
    }
    //On voit si le processus est mort ou si l'on a déjà trouvé un processus
    if (!procSuivant && (table[j].etat != 3)) {
      //Si non, on cherche si le processus est endormi et c'est le moment de le reveiller
      if ((table[j].etat == 2) && (nbr_secondes() >= table[j].heure)) {
	Actif = &table[j];
	procSuivant = true;
      }
      //On voit si le processus suivant est en état activable
      if (table[j].etat == 1) {
	Actif = &table[j];
	procSuivant = true;
      }
    }
  }
      
  //Si l'on a trouvé un processus à exécuter
  if (procSuivant) {
    //on change l'état du nouveau processus  à ELU
    Actif->etat = 0;
    //On voit si le processus antient vient de la fonction dors()
    if (Ancien->etat == 2) {
      if (nbr_secondes() >= Ancien->heure) {
	Ancien->etat = 1;
      }
      else {
	Ancien->etat = 2;
      }
    }
    else {
      //Si le processus antient n'est pas mort, on le met en ACTIVABLE
      if (Ancien->etat != 3) {
	Ancien->etat = 1;
      }
    }
  }

  //On montre dans l'écran les états et PIDs des processus
  affiche_etats();
  affiche_PID();

  //On fait le changement de contexte
  ctx_sw(Ancien->registres, Actif->registres);
}

/**
 *Fonction pour savoir le PID du processus actif
 *@return le PID du processus actif
 */
int32_t mon_pid(void) {
  return Actif->PID;
}

/**
 *Fonction pour savoir le nom du processus actif
 *@return le nom du processus actif
 */
char *mon_nom(void) {
  return Actif->nom;
}

//Processus IDLE
void idle() {
  for (;;) {
    sti();
    hlt();
    cli();
  }
}

//Processus 1
void proc1(void) {
  for (int32_t i = 0; i < 2; i++) {
    printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
	   mon_nom(), mon_pid());
    dors(2);
  }
  printf("[temps = %u] processus %s meurs\n", nbr_secondes(),
	 mon_nom());
}

//Processus 2
void proc2(void) {
  for (int32_t i = 0; i < 2; i++) {
    printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
	   mon_nom(), mon_pid());
    dors(3);
  }
  printf("[temps = %u] processus %s meurs\n", nbr_secondes(),
	 mon_nom());
}

//Processus 3
void proc3(void) {
  for (int32_t i = 0; i < 2; i++) {
    printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
	   mon_nom(), mon_pid());
    dors(5);
  }
  printf("[temps = %u] processus %s meurs\n", nbr_secondes(),
	 mon_nom());
}

//Processus pour créer des nouveaux processus
void proc_creator(void) {
  for (int i = 0; i < 5; i++) {
    printf("[temps = %u] %s pid = %i cree des processus\n", nbr_secondes(),
	   mon_nom(), mon_pid());
    cree_processus (&proc1, "proc 1");
    cree_processus (&proc2, "proc 2");
    cree_processus (&proc3, "proc 3");
    dors(15);
  }
}

/**
 *Fonction pour terminer un processus. 
*/
void fin_processus(void) {
  Actif->etat = 3;
  ordonnance();
}

/**
 *Fonction pour créer les processus.
 *@param Pointeur vers la direction du processus
 *@param Nom du processus
 *@return le PID du processus créé, ou -1 si l'on veut créer plus de processus que le nombre max de processus
 */
int32_t cree_processus (void (*code)(void), char *nom) {
  if (NbProc < MAXPROCESSUS) {
    table[NbProc].nom = nom;
    table[NbProc].pile[TAILLE - 2] = (uint32_t) code;
    table[NbProc].pile[TAILLE - 1] = (uint32_t) fin_processus;
    table[NbProc].registres[1] = (uint32_t) &table[NbProc].pile[TAILLE - 2];
    table[NbProc].PID = NbProc;
    table[NbProc].etat = 1;
    NbProc += 1;
    return table[NbProc-1].PID;
  } 
  else {
    int ProcMort = MAXPROCESSUS;
    for (int i = 0; i < MAXPROCESSUS; i++) {
      if (table[i].etat == 3) {
	ProcMort = i;
      }
    }
    if (ProcMort == MAXPROCESSUS) {
      return -1;
    }
    else {
      table[ProcMort].nom = nom;
      table[ProcMort].pile[TAILLE - 2] = (uint32_t) code;
      table[ProcMort].pile[TAILLE - 1] = (uint32_t) fin_processus;
      table[ProcMort].registres[1] = (uint32_t) &table[ProcMort].pile[TAILLE - 2];
      table[ProcMort].PID = ProcMort;
      table[ProcMort].etat = 1;
      return table[ProcMort-1].PID;
    }
  }
}

/**
 *Fonction pour créer le processus IDLE
 *Quand on le crée, on met tous les PIDS du table à MAXPROCESSUS
 *Un PID qui va permetre de savoir si c'est déjà créé ou pas
 */
void cree_Idle () {
  table[0].PID = 0; //processus idle
  table[0].nom = "idle";
  table[0].etat = 0;
  Actif = &table[0];
  
  for (int i=1; i<MAXPROCESSUS; i++) {
    table[i].PID = MAXPROCESSUS;
  }
}

/**
 *Fonction pour dormir les processus.
 *@param Met l'heure de reveille après ces secondes
 */
void dors(uint32_t nbr_secs) {
  Actif->heure = nbr_secondes() + nbr_secs;
  Actif->etat = 2;

  ordonnance();
}

/**
 *Fonction pour afficher les états des processus
 */
void affiche_etats(void) {
  uint16_t pos = posCurseur ();
  uint32_t col =  pos%80;
  uint32_t lig =  pos/80;

  for (int i = 0; i < NbProc; i++) {
    place_curseur(1+i,65);
    printf("%s etat=%i",
	   table[i].nom, table[i].etat);
  }

  place_curseur(lig,col);
}

/**
 *Fonction pour afficher les PIDs des processus
 */
void affiche_PID(void) {
  uint16_t pos = posCurseur ();
  uint32_t col =  pos%80;
  uint32_t lig =  pos/80;

  for (int i = 0; i < NbProc; i++) {
    place_curseur(1+NbProc+i,65);
    printf("%s PID=%i",
	   table[i].nom, table[i].PID);
  }

  place_curseur(lig,col);
}
