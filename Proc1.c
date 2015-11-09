#include "temps.h"

#include "console.h"
#include "inttypes.h"
#include "string.h"
#include "cpu.h"
#include "stdbool.h"
#include "stdio.h"
#include "ecran.h"

extern void ctx_sw(uint32_t* ancien, uint32_t* vouveau);

#define TAILLE 512

typedef struct processus {
  int PID;
  char * nom;
  int etat; //0 elu, 1 activable
  uint32_t registres [5];
  uint32_t pile [TAILLE];
}processus1;

struct processus table [2];

void idle(void) {
  for (int i = 0; i < 3; i++) {
    printf("[idle] je tente de passer la main a proc1...\n");
    ctx_sw(&table[0].registres[0], &table[1].registres[0]);
    printf("[idle] proc1 m’a redonne la main\n");
  }
  printf("[idle] je bloque le systeme\n");
  hlt();
}

void proc1(void) {
  for (;;) {
    printf("[proc1] idle m’a donne la main\n");
    printf("[proc1] je tente de lui la redonner...\n");
    ctx_sw(&table[1].registres[0], &table[0].registres[0]);
  }
}

void initial () {
  table[0].PID = 0; //processus idle
  table[0].nom = "idle";
  table[0].etat = 0; 

  table[1].PID = 1; //processus 1
  table[1].nom = "proc1";
  table[1].etat = 1;
  table[1].pile[TAILLE - 1] = (uint32_t) &proc1;
  table[1].registres[1] = (uint32_t) &table[1].pile[TAILLE - 1];
}
