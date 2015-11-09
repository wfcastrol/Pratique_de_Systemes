#ifndef _ecran_H
#define _ecran_H

#include "console.h"
#include "inttypes.h"
#include "string.h"
#include "cpu.h"

uint16_t *ptr_mem(uint32_t lig, uint32_t col);
void ecrit_car(uint32_t lig, uint32_t col, char c);
void place_curseur(uint32_t lig, uint32_t col);
void efface_ecran(void);
int16_t posCurseur (void);
void traite_car(char c);
void defilement(void);

#endif
