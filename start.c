#include "cpu.h"
#include "inttypes.h"
#include "stdio.h"
#include "ecran.h"
#include "temps.h"
#include "Proc1.h"

//Rendre visible le prototype
extern void traitant_IT_32();

//Initialisation de la variable de clock
double clk = 0;

void kernel_start(void)
{ 
  //efface_ecran;
  efface_ecran();
  printf("Hola");
  printf("\b");
  printf("o");
  
  //initialisation
  /*init_traitant_IT (31, traitant_IT_32);
  init_traitant_IT (32, traitant_IT_32);
  masque_IRQ(0,false);
  freq_clock();*/
  
  // démasquage des interruptions externes
  //sti();
  
  //Initialisation des structures de processus
  initial();
  
  //demarrage du processus par defaut
  idle();
  
  //Boucle d'attente
  while (1) {
      hlt();
  }
}
