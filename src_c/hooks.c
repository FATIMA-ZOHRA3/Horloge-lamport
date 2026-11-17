#include <stdio.h>
#include "hooks.h"

/* Version PROVISOIRE : affiche seulement les evenements.
 * TODO (Membre 2) : appeler horloges.c et journal.c ici. */

void hook_debut(int moi, int n) { (void)moi; (void)n; }

void hook_local(int moi) {
    printf("[P%d] LOCAL\n", moi);
}

void hook_envoi(int moi, int dest, Message* m) {
    printf("[P%d] ENVOI id=%d vers P%d\n", moi, m->id, dest);
}

void hook_reception(int moi, const Message* m) {
    printf("[P%d] RECEPTION id=%d de P%d\n", moi, m->id, m->expediteur);
}

void hook_fin(int moi) { (void)moi; }