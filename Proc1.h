#ifndef _Proc1_H
#define _Proc1_H

#include "console.h"
#include "inttypes.h"
#include "string.h"
#include "cpu.h"
#include "stdbool.h"

void idle(void);
void proc1(void);
void proc2(void);
void proc3(void);
void proc_creator(void);
void dors(uint32_t nbr_secs);
int32_t cree_processus (void (*code)(void), char *nom);
void cree_Idle ();
void ordonnance(void);
int32_t mon_pid(void);
char *mon_nom(void);
void fin_processus(void);
void affiche_etats(void);
void affiche_PID(void);

#endif
