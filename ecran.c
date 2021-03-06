/**
 *Code fait par Walter Ferney CASTRO LOPEZ
 *ENSIMAG - INP, Grenoble. 2015
 */

#include "ecran.h"

#include "console.h"
#include "inttypes.h"
#include "string.h"
#include "cpu.h"
#include "stdio.h"
#include "segment.h"

/**
 *Fonction retournant le un pointeur sur la case mémoire correspondant aux coordonnées fournies.
 *@param Ligne à lire
 *@param Colonne à lire
 @return pointeur vers la case mémoire
*/
uint16_t *ptr_mem(uint32_t lig, uint32_t col) {
  return (uint16_t*) (0xB8000 + 2*(lig*80+col));
}

/**
 *Fonction pour écrir un charactère
 *@param Ligne à lire
 *@param Colonne à lire
 *@param Charactère à écrire
*/
void ecrit_car(uint32_t lig, uint32_t col, char c) {
  uint16_t* dirMem = ptr_mem(lig, col);
  uint8_t ASCIIc = c;
  uint8_t format = 14;
  uint16_t ecrit = (format << 8) + ASCIIc;
  *dirMem = ecrit;
}

/**
 *Fonction qui place le curseur à la position donnée
 *@param Ligne où l'on veut placer le courseur
 *@param Colonne où l'on veut placer le courseur
 */
void place_curseur(uint32_t lig, uint32_t col) {
  if (col > 79) {
    col = 0;
    lig += 1;
  }

  if (lig > 24) {
    defilement();
  }
  else {
    uint16_t pos = (uint16_t) (col + (lig * 80));
    uint8_t posBasse = pos & 0b11111111;
    uint8_t posHaute = (pos) >> 8;
    uint8_t partBasse = (uint8_t) 0x0F;
    uint8_t partHaute = (uint8_t) 0x0E;
    uint16_t portCommande = (uint16_t) 0x3D4;
    uint16_t portDonnees = (uint16_t) 0x3D5;
    
    outb (partBasse, portCommande);
    outb (posBasse, portDonnees);
    outb (partHaute, portCommande);
    outb (posHaute, portDonnees);
  }
}

/**
 *Fonction pour éffacer l'écran.
 */
void efface_ecran(void) {
  for (uint32_t i=0; i<25; i++) {
    for (uint32_t j=0; j<80; j++) {
      char Space = ' ';
      ecrit_car (i, j, Space);
      if(i!=24 && j!=79) {
	place_curseur(i,j+1);
      }
    }
  }
  place_curseur (0,0);
}

/**
 *Fonction qui retourne la position du courseur
 *@return la position du courseur
 */
int16_t posCurseur (void){
  uint16_t portCommande = (uint16_t) 0x3D4;
  uint16_t portDonnees = (uint16_t) 0x3D5;
  uint8_t partBasse = (uint8_t) 0x0F;
  uint8_t partHaute = (uint8_t) 0x0E;

  outb (partBasse, portCommande);
  uint8_t posBasse = inb (portDonnees);
  outb (partHaute, portCommande);
  uint8_t posHaute = inb (portDonnees);

  uint16_t pos = (posHaute << 8) + posBasse;
  return pos;
}

/**
 *Fonction pour traiter les charactères de controle ou pour écrire
 *@param charactère à écrire
 */
void traite_car(char c) {
  uint16_t pos = posCurseur ();
  uint32_t col =  pos%80;
  uint32_t lig =  pos/80;
  uint8_t ASCIIc = c;
  
  if (ASCIIc > 31 && ASCIIc < 127) {
    ecrit_car (lig, col, c);
    place_curseur (lig, col+1);
  }
  
  else if (ASCIIc == 8) {
    if (col != 0) {
      place_curseur (lig, col-1);
    }
  }
  
  else if (ASCIIc == 9) {
    col +=1;
    while (col%8 != 0) {
      col += 1;
    }
    place_curseur (lig, col);
  }
  
  else if (ASCIIc == 10) {
    lig += 1;
    col = 0;
    place_curseur(lig,col);
  }

  else if (ASCIIc == 12) {
    efface_ecran();
  }

  else if (ASCIIc == 13) {
    col = 0;
    place_curseur(lig,col);
  }
}

/**
 *Fonction pour défiler les charactères
 */
void defilement1(void) {
  for (uint32_t i=1; i<25; i++) {
    for (uint32_t j=0; j<80; j++) {
      uint16_t* dirMem = ptr_mem(i, j);
      char Copier = (char) *dirMem;
      ecrit_car (i-1, j, Copier);
      char Space = ' ';
      ecrit_car (i, j, Space);
      if(i!=24 && j!=79) {
	place_curseur(i,j+1);
      }
    }
  }
}

//Fonction qui appelle défilement et place le courseur à la dernière ligne
void defilement(void) {
  defilement1();
  place_curseur(24, 0);
}

/**
 *Fonction pour imprimer une chaine de charactères
 *@param chaine de charactères
 *@param taille de la chaine
 */
void console_putbytes(const char *chaine, int taille) {
  for (uint32_t i=0; i<taille; i++) {
    traite_car (chaine[i]);
  }
}
