/**
 *Code fait par Walter Ferney CASTRO LOPEZ
 *ENSIMAG - INP, Grenoble. 2015
 */

#include "temps.h"

#include "ecran.h"
#include "console.h"
#include "inttypes.h"
#include "string.h"
#include "cpu.h"
#include "stdio.h"
#include "segment.h"
#include "Proc1.h"


//Des constantes pour le calcul de la fréquence
#define QUARTZ 0x1234DD
#define CLOCKFREQ 50

//Des constantes de temps
extern double clk;
int sec;

/**
 *Fonction qui retourne le nombre de secondes dès l'exécution
 *@return nombre de secondes
 */
int nbr_secondes() {
  return sec;
}

/**
 *Fonction pour montrer l'horloge dans l'écran
 *@param chaine de charactères à imprimer
 *@param longeur de la chaine de charactères
 */

void display_clock (char c [], int Size){
  uint16_t pos = posCurseur ();
  uint32_t col =  pos%80;
  uint32_t lig =  pos/80;
  char Space = ' ';

  //erases everything that is in the space of the clock
  for (uint32_t i=65; i<80; i++) {
    for (uint32_t j=0; j<11; j++) {
    ecrit_car (j, i, Space);
    place_curseur (0,1+i);
    }
  }

  //displays the clock
  for (uint32_t i=0; i<Size; i++) {
    ecrit_car (0, 70+i, c[i]);
    place_curseur (0,71+i);
  }
  place_curseur(lig,col); 
}

/**
 *Fonction traitant de l'intérruption de l'horloge
 */

void tic_PIT (void) {
  outb (0x20, 0x20);
  int min;
  int hr;

  clk += 1;
  sec = (clk)/CLOCKFREQ;
  min = (sec/60);
  hr = (min)/60;
  sec = sec%60;
  min = min%60;
  
  char buffer [8];
  int n = sprintf (buffer, "%d:%d:%d", hr, min, sec);
  display_clock (buffer, n);

  ordonnance();
}

/**
 *Fonction pour initialiser l'intérruption 32 des vecteurs d'interruption
 */

void init_traitant_IT(int32_t num_IT, void (*traitant)(void)) {

  uint32_t poids_faibles = (uint32_t) traitant & 0xFFFF; //16 poids faibles de l'adresse du traitant
  uint32_t poids_forts = (uint32_t) (traitant) >> 16 ; //16 poids forts de l'adresse du traitant

  uint32_t premier_mot = ((uint32_t) KERNEL_CS << 16) + poids_faibles;
  uint32_t deuxieme_mot = (poids_forts << 16) + (uint32_t) 0x8E00;

  uint32_t *adress1 = (uint32_t *) (0x1000 + num_IT*8); //Chaque entrée ocupe 2 mots de 4 octes, donc on doit avancer de 8 en 8
  uint32_t *adress2 = (uint32_t *) (0x1000 + num_IT*8 + 4);

  *adress1 = premier_mot;
  *adress2 = deuxieme_mot;
}

/**
 *Fonction pour régler la fréquence de l'horloge programmable
 */
void freq_clock () {
  outb (0x43, 0x34);

  uint16_t frequence = (uint16_t) (QUARTZ/CLOCKFREQ);

  uint8_t poids_faibles_frequence = frequence & 0xFF;
  uint8_t poids_forts_frequence = frequence >> 8;

  outb (poids_faibles_frequence, 0x40);
  outb (poids_forts_frequence, 0x40);
}

/**
 *Démasquage des intérruptions pour autoriser les signeaux 
 *en provenence de l'horloge
 *@param le numero de l'IRQ
 *@param l'indicateur disant si masquer ou démasquer
 */
void masque_IRQ(uint32_t num_IRQ, bool masque) {
  uint8_t valeur_masque = inb (0x21); 
  uint8_t valeur;

  if (masque) {
    valeur = (0x1 << num_IRQ) | valeur_masque;
  }
  else {
    valeur = ~(0x1 << num_IRQ) & valeur_masque;
  }
  
  outb (valeur, 0x21);
}
