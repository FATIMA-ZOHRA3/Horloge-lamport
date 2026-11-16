#ifndef SCENARIO_H
#define SCENARIO_H

#include "process.h"

#define MAX_ACTIONS 256

#define ACT_LOCAL 'L'
#define ACT_ENVOI 'S'
#define ACT_RECEP 'R'

/* Une action du scenario pour CE processus */
typedef struct {
    char type;   /* 'L', 'S' ou 'R' */
    int  dest;   /* destinataire pour 'S', sinon 0 */
} Action;

typedef struct {
    Action actions[MAX_ACTIONS];
    int    nb;
} Scenario;

/* Lit le fichier et ne garde que les lignes du processus 'moi'.
 * Format d'une ligne : <processus> <L | S k | R>   (# = commentaire).
 * Retourne 1 si le fichier est valide, 0 sinon (message d'erreur affiche). */
int  scenario_charger(Scenario* sc, const char* chemin, int moi, int n);

/* Execute les actions dans l'ordre, en appelant les hooks (voir hooks.h).
 * Retourne 1 si tout s'est bien passe. */
int  scenario_executer(const Scenario* sc, Processus* p);

#endif